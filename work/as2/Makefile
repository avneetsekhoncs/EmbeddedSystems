# Makefile for building embedded application.
# by Brian Fraser

# Edit this file to compile extra C files into their own programs.
# TARGETS= main sampler


# PUBDIR = $(HOME)/cmpt433/public/myApps
# OUTDIR = $(PUBDIR)
# CROSS_TOOL = arm-linux-gnueabihf-
# CC_CPP = $(CROSS_TOOL)g++
# CC_C = $(CROSS_TOOL)gcc

# CFLAGS = -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror -pthread
# # -pg for supporting gprof profiling.
# CFLAGS += -pg



# # Convert the targets to full-path executables to delete.
# OUTPUTS= $(addprefix $(OUTDIR)/, $(TARGETS))

# all: $(TARGETS)

# $(TARGETS):
# 	$(CC_C) $(CFLAGS) $@.c -o $(OUTDIR)/$@ -lpthread


# clean:
# 	rm -f $(OUTPUTS)


####################################################################
########## NEW ONE FROM BRIAN ######################################
####################################################################


# Makefile for building embedded application.
# by Brian Fraser

OUTFILE = main
OUTDIR = $(HOME)/cmpt433/public/myApps

CROSS_COMPILE = arm-linux-gnueabihf-
CC_CPP = $(CROSS_COMPILE)g++
CC_C = $(CROSS_COMPILE)gcc

# Indicate using POSIX standard (at least 2008+)
CFLAGS = -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror -pthread
SOURCES = main.c sampler.c potDriver.c segDisplay.c

app:
	$(CC_C) $(CFLAGS) $(SOURCES) -o $(OUTDIR)/$(OUTFILE)

clean:
	rm $(OUTDIR)/$(OUTFILE)

