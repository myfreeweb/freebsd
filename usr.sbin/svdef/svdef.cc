#include <sys/capsicum.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include <fcntl.h>
#include <stdlib.h>
#include <ucl++.h>
#include <unistd.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace fs = std::filesystem;
using ucl::Ucl;

static void
replace_at(fs::path &defname, int dfd, const char *name, const char *tmpname,
    const std::string &content, bool &changed)
{
	int oldfd = openat(dfd, name, O_RDONLY);
	if (oldfd == -1 && errno != ENOENT) {
		std::cerr << defname << ": could not open file " << name << ": "
			  << strerror(errno) << std::endl;
		exit(-1);
	}
	if (oldfd != -1) {
		struct stat sb;
		if (fstat(oldfd, &sb) == -1) {
			std::cerr << defname << ": could not stat file " << name
				  << ": " << strerror(errno) << std::endl;
			exit(-1);
		}
		bool diff = sb.st_size != content.size();
		if (!diff) {
			const char *oldcontent = nullptr;
			if ((oldcontent = (const char *)mmap(nullptr,
				 sb.st_size, PROT_READ, MAP_PRIVATE, oldfd,
				 0)) == MAP_FAILED) {
				std::cerr << defname << ": could not map file "
					  << name << ": " << strerror(errno)
					  << std::endl;
				exit(-1);
			}
			diff = strncmp(content.c_str(), oldcontent,
				   content.size()) != 0;
			if (munmap((void *)oldcontent, sb.st_size) == -1) {
				std::cerr << defname
					  << ": could not unmap file " << name
					  << ": " << strerror(errno)
					  << std::endl;
				exit(-1);
			}
		}
		if (close(oldfd) == -1) {
			std::cerr << defname << ": could not close file "
				  << name << ": " << strerror(errno)
				  << std::endl;
			exit(-1);
		}
		if (!diff) {
			return;
		}
	}
	int fd = openat(dfd, tmpname, O_WRONLY | O_CREAT, 0744);
	if (fd == -1) {
		std::cerr << defname << ": could not open file " << tmpname
			  << ": " << strerror(errno) << std::endl;
		exit(-1);
	}
	if (write(fd, content.c_str(), content.size()) != content.size()) {
		std::cerr << defname << ": could not write file " << tmpname
			  << ": " << strerror(errno) << std::endl;
		exit(-1);
	}
	if (close(fd) == -1) {
		std::cerr << defname << ": could not close file " << tmpname
			  << ": " << strerror(errno) << std::endl;
		exit(-1);
	}
	if (renameat(dfd, tmpname, dfd, name) == -1) {
		std::cerr << defname << ": could not move file " << tmpname
			  << " -> " << name << ": " << strerror(errno)
			  << std::endl;
		exit(-1);
	}
	changed = true;
}

int
main(int argc, char *argv[])
{
	if (argc < 2) {
		std::cerr << "usage: " << argv[0] << " [path]" << std::endl;
		return -1;
	}

	fs::path defpath((std::string(argv[1]))), defname = defpath.filename();

	int deffd = openat(AT_FDCWD, argv[1], O_RDONLY);
	if (deffd == -1) {
		std::cerr << defname
			  << ": could not open definition: " << strerror(errno)
			  << std::endl;
		return -1;
	}

	int svcsfd = openat(AT_FDCWD, "/service", O_DIRECTORY);
	if (svcsfd == -1) {
		std::cerr << defname << ": could not open services dir: "
			  << strerror(errno) << std::endl;
		return -1;
	}

	if (cap_enter() == -1) {
		std::cerr << defname
			  << ": could not cap_enter: " << strerror(errno)
			  << std::endl;
		return -1;
	}

	struct stat sb;
	if (fstat(deffd, &sb) == -1) {
		std::cerr << defname
			  << ": could not stat definition: " << strerror(errno)
			  << std::endl;
		return -1;
	}
	if (sb.st_size == 0) {
		return 0;
	}
	const char *deftxt = nullptr;
	if ((deftxt = (const char *)mmap(nullptr, sb.st_size, PROT_READ,
		 MAP_PRIVATE, deffd, 0)) == MAP_FAILED) {
		std::cerr << defname
			  << ": could not map definition: " << strerror(errno)
			  << std::endl;
		return -1;
	}

	std::map<std::string, std::string> uclvars;
	extern char **environ;
	for (char **current = environ; *current; current++) {
		std::string var(*current);
		auto sep = var.find('=');
		uclvars.emplace(var.substr(0, sep), var.substr(sep + 1));
	}
	std::string uclerr;
	Ucl def = Ucl::parse(deftxt, uclvars, uclerr);
	if (!uclerr.empty()) {
		std::cerr << defname
			  << ": could not parse definition: " << uclerr
			  << std::endl;
		return -1;
	}

	auto overridename = def[std::string("name")].string_value();
	auto runcmd = def[std::string("run")].string_value();
	auto preruncmd = def[std::string("before_run")].string_value();
	auto fincmd = def[std::string("finish")].string_value();
	auto nice = def[std::string("nice")].int_value();
	auto user = def[std::string("user")].string_value();
	if (runcmd.empty()) {
		std::cerr << defname << ": no run command in definition"
			  << std::endl;
		return -1;
	}

	if (!overridename.empty()) {
		defname = overridename;
	}

	if (mkdirat(svcsfd, defname.c_str(), 0755) == -1 && errno != EEXIST) {
		std::cerr << defname << ": could not create service dir: "
			  << strerror(errno) << std::endl;
		return -1;
	}
	int svcdirfd = openat(svcsfd, defname.c_str(), O_DIRECTORY);
	if (svcdirfd == -1) {
		std::cerr << defname
			  << ": could not open service dir: " << strerror(errno)
			  << std::endl;
		return -1;
	}

	if (mkdirat(svcdirfd, "log", 0755) == -1 && errno != EEXIST) {
		std::cerr << defname
			  << ": could not create log dir: " << strerror(errno)
			  << std::endl;
		return -1;
	}
	int logdirfd = openat(svcdirfd, "log", O_DIRECTORY);
	if (logdirfd == -1) {
		std::cerr << defname
			  << ": could not open log dir: " << strerror(errno)
			  << std::endl;
		return -1;
	}

	bool changed_fin = false, changed_run = false, changed_log = false;

	if (!fincmd.empty()) {
		std::stringstream finscpt;
		finscpt << "#!/bin/sh" << std::endl;
		finscpt << "# generated by svdef #" << std::endl;
		finscpt << "exec " << fincmd << std::endl;
		replace_at(defname, svcdirfd, "finish", "finish.tmp",
		    finscpt.str(), changed_fin);
	} else {
		if (unlinkat(svcdirfd, "finish", 0) == -1 && errno != ENOENT) {
			std::cerr << defname
				  << ": could not remove finish script: "
				  << strerror(errno) << std::endl;
			return -1;
		}
	}

	std::stringstream logscpt;
	logscpt << "#!/bin/sh" << std::endl;
	logscpt << "# generated by svdef #" << std::endl;
	logscpt << "exec logger -t " << defname << std::endl;
	replace_at(defname, logdirfd, "run", "run.tmp", logscpt.str(),
	    changed_log);

	std::stringstream runscpt;
	runscpt << "#!/bin/sh" << std::endl;
	runscpt << "# generated by svdef #" << std::endl;
	runscpt << preruncmd << std::endl;
	runscpt << "exec chpst ";
	if (nice) {
		runscpt << "-n " << nice << " ";
	}
	if (!user.empty()) {
		runscpt << "-u " << nice << " ";
	}
	runscpt << runcmd << std::endl;
	replace_at(defname, svcdirfd, "run", "run.tmp", runscpt.str(),
	    changed_run);

	if (changed_log) {
		int ctlfd = openat(logdirfd, "supervise/control", O_WRONLY);
		if (ctlfd != -1) {
			write(ctlfd, "tcu", 3); // sv restart
		}
	}
	if (changed_run) {
		int ctlfd = openat(svcdirfd, "supervise/control", O_WRONLY);
		if (ctlfd != -1) {
			write(ctlfd, "tcu", 3); // sv restart
		}
	}

	return 0;
}
