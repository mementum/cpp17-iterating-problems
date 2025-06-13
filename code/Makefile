# Parameter to be overriden in the command line
std ?= c++17

# Collect problems
PROBLEMS := $(wildcard [0-9][0-9]-*)
PROBNUMS := $(shell echo $(PROBLEMS) | tr ' ' '\n' | cut -b1-2)

# Flags for each problem
PROBFLAGS := std=$(std)

# Default target
default: help

# "show" options
ifeq (show,$(filter show,$(MAKECMDGOALS)))
PROBFLAGS := $(PROBFLAGS) show

show: ;
endif

ifeq (showoutput,$(filter showoutput,$(MAKECMDGOALS)))
PROBFLAGS := $(PROBFLAGS) showoutput

showoutput: ;
endif

ifeq (showdiff,$(filter showdiff,$(MAKECMDGOALS)))
PROBFLAGS := $(PROBFLAGS) showdiff

showdiff: ;
endif


# Do all the problems with another make to ensure they are made
# even if some of them (as expected) fail. "all" could simply
# have had all the $(PROBNUMS) as dependencies if all succeeded.
all:
	@$(foreach var,$(PROBLEMS),$(MAKE) -C $(var) $(PROBFLAGS) all;)

clean:
	@$(foreach var,$(PROBLEMS),$(MAKE) -C $(var) clean;)

# Recipe to make a problem
%:
	$(eval probtarget := $(filter $@-%,$(PROBLEMS)))
	@[ -z "$(probtarget)" ] || echo "Making: $(probtarget)"
	@[ -z "$(probtarget)" ] || $(MAKE) -C $(probtarget) $(PROBFLAGS) all
	@[ -n "$(probtarget)" ] || echo "$@ NOT FOUND" && echo

# show the help text
help:
	@echo
	@echo "Compile all solutions of a problem or all problems"
	@echo
	@echo "Target:"
	@echo
	@echo "    A 2-digit number to make and test that problem (0x, 1x, ...)"
	@echo "    Example: 'make 03'"
	@echo
	@echo "If a problem with that numbering is found, all solutions will be compiled,"
	@echo "if needed be, executed with the problem input, and the output"
	@echo "compared to the expected output."
	@echo
	@echo "    SUCCESS or FAILED will be finally shown, as in:"
	@echo "    Example: 'Solution 03: SUCCESS'"
	@echo
	@echo "If the solution fails, the expected output, the actual output and"
	@echo "the difference will be shown."
	@echo
	@echo "Options:"
	@echo "  all          - make all problems (ignore solutions with errors)"
	@echo "  std=c++17    - c++ standard to use)"
	@echo ""
	@echo "  showoutput   - Force showing the output even on success "
	@echo "  showdiff     - Force showing the expected vs output diff even on success"
	@echo "  show         - Force showing the output and diff even on success"
	@echo "  clean        - Remove all executables"
	@echo "  help         - Show this help"
	@echo
