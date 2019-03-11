CC = gcc
LIBS = -lm

CFLAGS = -O2 -s

analysis := CFLAGS = -O2 -s -DTIMER -DANALYSIS

FSA = fsa
FSA_SRC = main.c fsa.c cvutil.c imageio.c

TSS = tss
TSS_SRC = main.c tss.c cvutil.c imageio.c

BSP = bsp
BSP_SRC = main.c bsp.c multilevel.c cvutil.c imageio.c

WINUP = winup
WINUP_SRC = main.c winup.c multilevel.c cvutil.c imageio.c

WINUP_TSS = winup_tss
WINUP_TSS_SRC = main.c winup_tss.c multilevel.c cvutil.c imageio.c

optimize analysis: $(FSA) $(TSS) $(BSP) $(WINUP) $(WINUP_TSS)

$(FSA): $(FSA_SRC)
	$(CC) $(CFLAGS) -o $(FSA) $(FSA_SRC) $(LIBS)

$(TSS): $(TSS_SRC)
	$(CC) $(CFLAGS) -o $(TSS) $(TSS_SRC) $(LIBS)

$(BSP): $(BSP_SRC)
	$(CC) $(CFLAGS) -o $(BSP) $(BSP_SRC) $(LIBS)

$(WINUP): $(WINUP_SRC)
#	$(CC) $(CFLAGS) -o $(WINUP) $(WINUP_SRC) $(LIBS)
	$(CC) $(CFLAGS) -DPREDICTION -o $(WINUP) $(WINUP_SRC) $(LIBS)

$(WINUP_TSS): $(WINUP_TSS_SRC)
	$(CC) $(CFLAGS) -o $(WINUP_TSS) $(WINUP_TSS_SRC) $(LIBS)

clean:
	rm $(FSA) $(TSS) $(BSP) $(WINUP) $(WINUP_TSS)
