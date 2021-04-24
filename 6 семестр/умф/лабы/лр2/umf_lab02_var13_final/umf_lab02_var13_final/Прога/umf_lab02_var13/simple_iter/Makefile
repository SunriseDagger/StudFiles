F95 = gfortran
DESTDIR = bin/Debug
OBJDIR = obj/Debug
SOURCE = simple_iter.f95
OBJECT = simple_iter.o
BINARY = simple_iter
RMDIR = bin obj
F95FLAGS = -J$(OBJDIR)/ -cpp -std=f2008 -fimplicit-none -fdefault-integer-8 -fdefault-double-8 -fdefault-real-8 -Wfatal-errors -W -Wextra -Wall

.SILENT:clean run
.PHONY:all clean run

all:
	mkdir -p $(DESTDIR) $(OBJDIR)
	$(F95) $(F95FLAGS) -c $(SOURCE) -o $(OBJDIR)/$(OBJECT)
	$(F95) -o $(DESTDIR)/$(BINARY) $(OBJDIR)/$(OBJECT)
	rm $(OBJDIR)/$(OBJECT)
clean:
	rm -r $(RMDIR) 2>/dev/null || true
run:
	$(DESTDIR)/$(BINARY)

