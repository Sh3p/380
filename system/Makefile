########################################################
# Dr. Zoppetti: Makefile example
########################################################

########################################################
# Variable definitions
########################################################
# C compiler
CC      = gcc

# C compiler flags
CFLAGS = -g -Wall -Wextra -Werror 
#CFLAGS = -O3 -Wall -Wextra -Werror 

# Linker flags
LDFLAGS  := 

# Libraries we're using, prefaced with "-l"
LDLIBS   := 

#############################################################
# Rules
#   Rules have the form
#   target : prerequisites
#   	  recipe
#############################################################

MySystem : MySystem.o
	$(LINK.o) $(LDFLAGS) $^ $(LDLIBS) -o $@

MySystem.o : MySystem.c

#############################################################

.PHONY : clean

clean :
	@$(RM) MySystem a.out core
	@$(RM) *.o *~ *.d

#############################################################
