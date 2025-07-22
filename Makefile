# NovaType Makefile
# GTK4 + libadwaita word processor

# Program name
PROGRAM = NovaType

# Source files
SOURCES = main.c

# Object files
OBJECTS = $(SOURCES:.c=.o)

# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -std=c11 -O2
DEBUG_CFLAGS = -Wall -Wextra -std=c11 -g -DDEBUG

# Package config for GTK4 and libadwaita
PKG_CONFIG = pkg-config
PACKAGES = gtk4 libadwaita-1

# Get flags from pkg-config
CFLAGS += $(shell $(PKG_CONFIG) --cflags $(PACKAGES))
LDFLAGS = $(shell $(PKG_CONFIG) --libs $(PACKAGES))

# Installation paths
PREFIX = /usr/local
BINDIR = $(PREFIX)/bin
DATADIR = $(PREFIX)/share
APPDIR = $(DATADIR)/applications
ICONDIR = $(DATADIR)/icons/hicolor

# Default target
all: $(PROGRAM)

# Build the main program
$(PROGRAM): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(PROGRAM) $(LDFLAGS)

# Compile source files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Debug build
debug: CFLAGS = $(DEBUG_CFLAGS) $(shell $(PKG_CONFIG) --cflags $(PACKAGES))
debug: $(PROGRAM)

# Clean build artifacts
clean:
	rm -f $(OBJECTS) $(PROGRAM)

# Install the program
install: $(PROGRAM)
	install -d $(DESTDIR)$(BINDIR)
	install -m 755 $(PROGRAM) $(DESTDIR)$(BINDIR)
	@echo "NovaType installed to $(DESTDIR)$(BINDIR)/$(PROGRAM)"

# Install with desktop entry (optional)
install-desktop: install
	install -d $(DESTDIR)$(APPDIR)
	@echo "[Desktop Entry]" > NovaType.desktop
	@echo "Name=NovaType" >> NovaType.desktop
	@echo "Comment=Modern word processor with SUNT format" >> NovaType.desktop
	@echo "Exec=$(BINDIR)/$(PROGRAM)" >> NovaType.desktop
	@echo "Icon=text-editor" >> NovaType.desktop
	@echo "Terminal=false" >> NovaType.desktop
	@echo "Type=Application" >> NovaType.desktop
	@echo "Categories=Office;WordProcessor;" >> NovaType.desktop
	@echo "MimeType=application/x-sunt;" >> NovaType.desktop
	install -m 644 NovaType.desktop $(DESTDIR)$(APPDIR)
	@echo "Desktop entry installed to $(DESTDIR)$(APPDIR)"

# Uninstall
uninstall:
	rm -f $(DESTDIR)$(BINDIR)/$(PROGRAM)
	rm -f $(DESTDIR)$(APPDIR)/NovaType.desktop
	@echo "NovaType uninstalled"

# Run the program
run: $(PROGRAM)
	./$(PROGRAM)

# Check dependencies
check-deps:
	@echo "Checking dependencies..."
	@$(PKG_CONFIG) --exists $(PACKAGES) || (echo "Missing dependencies. Install GTK4 and libadwaita development packages." && exit 1)
	@echo "All dependencies found!"

# Show help
help:
	@echo "NovaType Makefile"
	@echo ""
	@echo "Targets:"
	@echo "  all          - Build the program (default)"
	@echo "  debug        - Build with debug symbols"
	@echo "  clean        - Remove build artifacts"
	@echo "  install      - Install program to system"
	@echo "  install-desktop - Install with desktop entry"
	@echo "  uninstall    - Remove from system"
	@echo "  run          - Build and run the program"
	@echo "  check-deps   - Check if dependencies are available"
	@echo "  help         - Show this help message"
	@echo ""
	@echo "Variables:"
	@echo "  PREFIX       - Installation prefix (default: /usr/local)"
	@echo "  DESTDIR      - Destination directory for packaging"
	@echo "  CC           - C compiler (default: gcc)"
	@echo ""
	@echo "Examples:"
	@echo "  make                    # Build normally"
	@echo "  make debug              # Build with debug info"
	@echo "  make install PREFIX=/usr # Install to /usr instead of /usr/local"
	@echo "  make run                # Build and run"

# Declare phony targets
.PHONY: all debug clean install install-desktop uninstall run check-deps help

# Dependencies (automatically generated)
NovaType.o: NovaType.c
