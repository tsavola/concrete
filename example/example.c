#include <stdio.h>
#include <stdlib.h>

#include <concrete.h>

static void check(ConcreteError *error)
{
	if (error->type) {
		fprintf(stderr, "%s\n", error->message);
		exit(2);
	}
}

int main(int argc, char **argv)
{
	const char *filename = EXAMPLE_BYTECODE;
	FILE *file;
	long size;
	char *data;

	if (argc > 1)
		filename = argv[1];

	file = fopen(filename, "r");
	if (file == NULL)
		return 1;

	if (fseek(file, 0, SEEK_END) < 0)
		return 1;

	size = ftell(file);
	if (size < 0)
		return 1;

	if (fseek(file, 0, SEEK_SET) < 0)
		return 1;

	data = malloc(size);
	if (data == NULL)
		return 1;

	if (fread(data, 1, size, file) != size)
		return 1;

	fclose(file);

	ConcreteError error;
	ConcreteContext *context;
	bool executable;

	context = concrete_create(&error);
	check(&error);

	concrete_load(context, data, size, &error);
	check(&error);

	do {
		executable = concrete_execute(context, &error);
		if (error.type == CONCRETE_ERROR_CODE)
			fprintf(stderr, "%s\n", error.message);
		else
			check(&error);
	} while (executable);

	concrete_destroy(context);

	return 0;
}
