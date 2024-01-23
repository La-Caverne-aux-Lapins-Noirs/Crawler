#################################################################################

 ###############################################################################
 ## Jason Brillante "Damdoshi"                                                ##
 ## Hanged Bunny Studio 2014-2020                                             ##
 ##                                                                           ##
 ##                            - TechnoCentre -                               ##
 ##                                                                           ##
 ###############################################################################

#################################################################################

  SRC_DIR	=	../src/
  REPORT	?=	./report/
  COMPILER	?=	gcc
  LIB_TO_TEST	=	LD_LIBRARY_PATH=../

  INC_DIR	?=	/usr/include/

  LIB		=	-L$(HOME)/.froot/lib/ -L../ -L./ --whole-file -lcrawler -llapin		\
			-lsfml-graphics -lsfml-window -lsfml-audio -lsfml-system -lavcall -ldl -lm -lusb -lgcov -lstdc++
  FLAGS		?=	-fPIC -std=c11 -Wno-format-security -Wall -Wextra	\
			-O0 -Og -g -g3 -ggdb -fprofile-arcs -ftest-coverage	\
			--coverage -rdynamic -fno-omit-frame-pointer		\
			-fno-align-functions -fno-align-loops -DFTRACE 		\
			-Wno-unused-label

  RM		=	rm -rf
  ECHO		=	/bin/echo -e
  LOGFILE	=	errors~
  ERRLOG	=	2>> $(LOGFILE)
  FERRLOG	=	>> $(LOGFILE) 2>> $(LOGFILE)
  NEGATE	= 	> /dev/null 2> /dev/null

  HEADER	=	-I./../include/ -I$(HOME)/.froot/include

  DEFLT		=	"\033[00m"
  PINK		=	"\033[1;35m"
  GREEN		=	"\033[0;32m"
  TEAL		=	"\033[1;36m"
  RED		=	"\033[0;31m"

#################################################################################
## Source                                                                      ##
#################################################################################

  SRC		=	$(wildcard src/*.c)
  OBJ		=	$(basename $(SRC))

#################################################################################
## Rules                                                                       ##
#################################################################################

all:			rmlog $(REPORT)
$(OBJ):
			@$(COMPILER) $@.c -o $@ $(FLAGS)			\
			 $(HEADER) $(LIB) $(ERRLOG)
			@test -f $@ || $(ECHO) $(RED)  "[KO]" $@.c $(DEFLT)
			@test -f $@ && $(ECHO) "------\n$@ :" $(FERRLOG) &&	\
			 ($(LIB_TO_TEST) $@ $(FERRLOG) &&			\
			 $(ECHO) $(TEAL) "[OK]" $(GREEN)$(notdir $@)		\
			 $(DEFLT) ||						\
			 $(ECHO) $(RED) "[KO]" $(notdir $@) $(DEFLT)) ||	\
			 echo -n ""

$(REPORT):		$(OBJ)
			@lcov --directory . --directory $(SRC_DIR) -c		\
			 -o coverage_raw.info $(NEGATE) &&			\
			 lcov --remove coverage_raw.info '$(INC_DIR)*' '7/*'	\
			 -o coverage.info $(NEGATE) &&				\
			 genhtml -o $(REPORT) -t "TechnoCore" coverage.info |	\
			 grep "\%" ;						\
			 $(RM) coverage_raw.info

#################################################################################
## Misc                                                                        ##
#################################################################################

rmlog:
			@$(RM) $(LOGFILE)
clean:
			@find . -name "*.gc*" -delete
			@find . -name "*.info" -delete
			@$(RM) $(OBJ)
fclean:			clean
			@$(RM) $(REPORT)
re:			fclean all
