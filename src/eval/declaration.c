/**
* declaration.c
*/

#include <b.h>

static void
B_auto_scalar(StringC name)
{
	Symbol	scalar = 
	{
		.name  = name,
		.stype = STORAGE_AUTO,
		.vtype = VARIABLE_SCALAR,
		.size  = 1,
		.off   = -(B.auto_size + 1),
	};
	B_symbol_add(&scalar);
	B.auto_size += 1;
}

static void
B_auto_vector(StringC name, Size size)
{
	Symbol	vector = 
	{
		.name  = name,
		.stype = STORAGE_AUTO,
		.vtype = VARIABLE_VECTOR,
		.size  = size,
		.off   = -(B.auto_size + 1),
	};
	B_symbol_add(&vector);
	B.auto_size += size;
}

static void
B_auto_param(StringC name)
{
	Symbol	vector = 
	{
		.name  = name,
		.stype = STORAGE_AUTO,
		.vtype = VARIABLE_UNKNOWN,
		.size  = 1,
		.off   = B.param_size + 2,
	};
	B_symbol_add(&vector);
	B.param_size++;
}

static void
B_external_scalar(StringC name)
{
	Symbol	simple = 
	{
		.name  = name,
		.stype = STORAGE_EXTERN,
		.vtype = VARIABLE_SCALAR,
		.size  = 1,
	};
	B_symbol_add(&simple);
}

static void
B_external_vector(StringC name, Size size)
{
	Symbol	vector = 
	{
		.name  = name,
		.stype = STORAGE_EXTERN,
		.vtype = VARIABLE_VECTOR,
		.size  = size,
	};
	B_symbol_add(&vector);
}

static void
B_external_unknown(StringC name)
{
	Symbol	unknown = 
	{
		.name  = name,
		.stype = STORAGE_EXTERN,
		.vtype = VARIABLE_UNKNOWN,
	};
	B_symbol_add(&unknown);
}

static void
B_internal_scalar(StringC name)
{
	Symbol	scalar = 
	{
		.name  = name,
		.stype = STORAGE_INTERN,
		.vtype = VARIABLE_SCALAR,
		.size  = 1,
	};
	B_symbol_add(&scalar);
}

static void
B_function(StringC name)
{
	Symbol	function = 
	{
		.name  = name,
		.stype = STORAGE_EXTERN,
		.vtype = VARIABLE_FUNCTION,
		.size  = 0,
	};
	B_symbol_add(&function);
}


void
B_eval_auto_decl(StringC name, Size size)
{
	bool	scalar = !size;

	if (scalar)
		B_auto_scalar(name);
	else
		B_auto_vector(name, size);
}

void
B_eval_auto_list(void)
{
	CG_stack_reserve(B.auto_size * WORD_SIZE);

	B.auto_size = 0;
}

void
B_eval_extern_decl(StringC name)
{
	B_external_unknown(name);
}

void
B_eval_param_decl(StringC name)
{
	B_auto_param(name);
}

void
B_eval_param_list(void)
{
	B.param_size = 0;
}

void
B_eval_function_def(StringC name)
{
	B_function(name);
}

void
B_eval_intern_def(void)
{
	todo("code this: %s", __func__);
}

void
B_eval_simple_def(StringC name)
{
	B_external_scalar(name);
	CG_label(name, false);
	CG_data_scalar_list();
}

void
B_eval_vector_def(StringC name, Size size)
{
	B_external_vector(name, size);
	CG_label(name, true);
	CG_data_vector(size);
}

void
B_eval_ival(i32 imm, StringC sym)
{
	IVal	value = {0};

	if (sym)
		value.sym = sym;
	else
		value.imm = imm;
	arr_append(B.ivals, value);
}
