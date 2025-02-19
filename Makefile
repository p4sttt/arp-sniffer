TARGET = arpsnif 

OUTDIR = build/
BINDIR = bin/
SRCDIR = src/
INCDIR = include/

SRCS = main.c net.c utils.c
OBJS = $(SRCS:.c=.o)

CC = gcc
CCFLAGS = -std=gnu89 -pedantic -Wall -DCMAKE_EXPORT_COMPILE_COMMANDS=1

all: $(BINDIR)$(TARGET)

$(BINDIR)$(TARGET): $(addprefix $(OUTDIR), $(OBJS))
	@mkdir -p $(BINDIR)
	$(CC) $(addprefix $(OUTDIR), $(OBJS)) -o $(BINDIR)$(TARGET)

$(OUTDIR)%.o: $(SRCDIR)%.c
	@mkdir -p $(OUTDIR)
	$(CC) $(CCFLAGS) -I$(INCDIR) -c $< -o $@

clean:
	rm -rf $(BINDIR)
	rm -rf $(OUTDIR)
