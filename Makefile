# Source files
FORMAT_SRC = $(SRC) $(wildcard *.h)

# Lint files
format:
	clang-format -i $(FORMAT_SRC)