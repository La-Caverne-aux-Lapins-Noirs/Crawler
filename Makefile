#################################################################################

 ###############################################################################
 ## Jason Brillante "Damdoshi"                                                ##
 ## Hanged Bunny Studio 2014-2021                                             ##
 ##                                                                           ##
 ##                            - TechnoCentre -                               ##
 ##                                                                           ##
 ###############################################################################

#################################################################################
## Information about the project                                               ##
#################################################################################

  LIB		=	libcccrawler.a
  TESTLIB	=	libcccrawler.so

#################################################################################
## Building details                                                            ##
#################################################################################

  COMPILER	?=	gcc
  LINKER	?=	ar rcs
  TESTLINKER	?=	gcc -shared -fprofile-arcs $(LIB) -o

  BIN_DIR	?=	/usr/bin/
  LIB_DIR	?=	/usr/lib/
  INC_DIR	?=	/usr/include/
  ETC_DIR	?=	/etc/technocore/

  LIB		=	-llapin -ldl -lm -lstdc++ -lavcall	\
			-lgcov -rdynamic
  CONFIG	?=	$(FLAG) -fPIC -std=c11 -Wno-format-security		\
			-Wall -Wextra

  DEBUG		=	-O0 -Og -g -g3 -ggdb -fprofile-arcs -ftest-coverage	\
			--coverage -fno-omit-frame-pointer -fno-align-functions	\
			-fno-align-loops
  PRODUCTION	=	-O3 -ffast-math -march=native -DNDEBUG

  ifeq ($(RELEASE), 1)
    MODE_NAME	=	"Build mode: release"
    PROFILE	=	$(PRODUCTION)
  else
    MODE_NAME	=	"Build mode: debug"
    PROFILE	=	$(DEBUG)
  endif

  CP		=	cp -r
  RM		=	rm -f
  ECHO		=	/bin/echo -e
  LOGFILE	=	errors~
  ERRLOG	=	2>> $(LOGFILE)

  HEADER	=	-I./include/

  DEFLT		=	"\033[00m"
  PINK		=	"\033[1;35m"
  GREEN		=	"\033[0;32m"
  TEAL		=	"\033[1;36m"
  RED		=	"\033[0;31m"

#################################################################################
## Source                                                                      ##
#################################################################################

  SRC		=	$(wildcard src/*.c) $(wildcard src/*/*.c)
  OBJ		=	$(SRC:.c=.o)
  CNF		=	$(wildcard *.dab) $(wildcard *.d)
  INC		=	./include/technocore_api.h

  LIBOBJ	=	$(filter-out src/main.o, $(OBJ))

#################################################################################
## Rules                                                                       ##
#################################################################################

all:			build check
build:			rmlog $(NAME)
$(NAME):		$(OBJ)
			@$(LINKER) $(NAME) $(OBJ) $(LIB) $(ERRLOG) &&		\
			 $(ECHO) $(TEAL) "[OK]" $(GREEN) $(NAME) $(DEFLT) ||	\
			 $(ECHO) $(RED)  "[KO]" $(NAME) $(DEFLT)
			@echo $(MODE_NAME)
$(TESTLIB):		$(OBJ)
			@$(TESTLINKER) $(TESTLIB) $(LIBOBJ) $(ERRLOG) &&	\
			 $(ECHO) $(TEAL) "[OK]" $(GREEN) $(TESTLIB) $(DEFLT) || \
			 $(ECHO) $(RED)  "[KO]" $(TESTLIB) $(DEFLT)
			@echo $(MODE_NAME)
.c.o:
			@$(COMPILER) -c $< -o $@ $(PROFILE) $(CONFIG)		\
			 $(HEADER) $(ERRLOG) &&					\
			 $(ECHO) $(TEAL) "[OK]" $(GREEN) $< $(DEFLT) ||	\
			 $(ECHO) $(RED)  "[KO]" $< $(DEFLT)

#################################################################################
## Misc                                                                        ##
#################################################################################

check:			$(TESTLIB)
			@(cd test/ && $(MAKE) --no-print-directory)

install:
			@$(CP) $(NAME) $(BIN_DIR)
			@$(CP) $(CNF) $(ETC_DIR)
			@$(CP) $(INC) $(INC_DIR)
rmlog:
			@$(RM) $(LOGFILE)
clean:
			@find . -name "*.gc*" -delete
			@$(RM) $(OBJ) &&					\
			 $(ECHO) $(GREEN) "Object file deleted" $(DEFLT) ||	\
			 $(ECHO) $(RED) "Error in clean rule!" $(DEFLT)
			@(cd test/ && $(MAKE) --no-print-directory clean)
fclean:			clean
			@find .     -name "*.so" -delete			\
				-or -name "*.exe" -delete			\
				-or -name "*.a" -delete				\
				-or -name "*.o" -delete				\
				-or -name "*~" -delete
			@$(RM) $(NAME) &&					\
			 $(ECHO) $(GREEN) "Program deleted!" $(DEFLT) ||	\
			 $(ECHO) $(RED) "Error in fclean rule!" $(DEFLT)
			@$(RM) $(TESTLIB) &&					\
			 $(ECHO) $(GREEN) "Library deleted!" $(DEFLT) ||	\
			 $(ECHO) $(RED) "Error in fclean rule!" $(DEFLT)
			@(cd test/ && $(MAKE) --no-print-directory fclean)
re:			fclean all
erase:
			@$(RM) $(LOGFILE)
