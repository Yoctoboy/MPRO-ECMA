SYSTEM     = x86-64_sles10_4.1
LIBFORMAT  = static_pic

#------------------------------------------------------------
#
# When you adapt this makefile to compile your CPLEX programs
# please copy this makefile and set CPLEXDIR and CONCERTDIR to
# the directories where CPLEX and CONCERT are installed.
#
#------------------------------------------------------------

CPLEXDIR      = /opt/ibm/ILOG/CPLEX_Studio125/cplex
CONCERTDIR    = /opt/ibm/ILOG/CPLEX_Studio125/concert
# ---------------------------------------------------------------------
# Compiler selection
# ---------------------------------------------------------------------

CCC = g++


# ---------------------------------------------------------------------
# Compiler options
# ---------------------------------------------------------------------

CCOPT = -m64 -O -fPIC -fno-strict-aliasing -fexceptions -DNDEBUG -DIL_STD -ggdb

# ---------------------------------------------------------------------
# Link options and libraries
# ---------------------------------------------------------------------

CPLEXLIBDIR   = $(CPLEXDIR)/lib/$(SYSTEM)/$(LIBFORMAT)
CONCERTLIBDIR = $(CONCERTDIR)/lib/$(SYSTEM)/$(LIBFORMAT)
CPLEXINCDIR   = $(CPLEXDIR)/include
CONCERTINCDIR = $(CONCERTDIR)/include
CCFLAGS = $(CCOPT) -I$(CONCERTINCDIR) -I$(CPLEXINCDIR)
CCLNDIRS  = -L$(CPLEXLIBDIR) -L$(CONCERTLIBDIR)
CCLNFLAGS = -lconcert -lilocplex -lcplex -lm -pthread


# ------------------------------------------------------------

all: colGen

branchCut: branchCut.o includesBranchCut.o
	g++ $(CCFLAGS) $(CCLNDIRS) -o branchCut branchCut.o includesBranchCut.o $(CCLNFLAGS)

branchCut.o: branchCut.cpp
	g++ -c $(CCFLAGS) branchCut.cpp -o branchCut.o

includesBranchCut.o: includesBranchCut.cpp
	g++ -c $(CCFLAGS) includesBranchCut.cpp -o includesBranchCut.o

colGen: colGen.o includesColGen.o
	g++ $(CCFLAGS) $(CCLNDIRS) -o colGen colGen.o includesColGen.o $(CCLNFLAGS)

colGen.o: colGen.cpp
	g++ -c $(CCFLAGS) colGen.cpp -o colGen.o

includesColGen.o: includesColGen.cpp
	g++ -c $(CCFLAGS) includesColGen.cpp -o includesColGen.o

clean:
	rm -rf *.o

# Local Variables:
# mode: makefile
# End:
