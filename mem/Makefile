########################################################
# Dr. Zoppetti: Makefile example
########################################################

########################################################
# Variable definitions
########################################################
# C compiler
CC      = gcc

# C compiler flags
#CFLAGS = -g -Wall -Wextra -Werror 
#CFLAGS = -O3 -Wall -Wextra -Werror 
CFLAGS = -Wall

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

MyCopy : MyCopy.o
	$(LINK.o) $(LDFLAGS) $^ $(LDLIBS) -o $@

MyCopy.o : MyCopy.c

#############################################################

.PHONY : clean

clean :
	@$(RM) Exec a.out core
	@$(RM) *.o *~ *.d

#############################################################