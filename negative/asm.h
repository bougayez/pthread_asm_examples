#include <stdio.h>
#include <stdlib.h>

int asm_add(int a, int b)
{
	int c = 0;

	__asm__ volatile (
		"movl %1, %%eax   \n\t" // Move b into the EAX register
		"addl %2, %%eax   \n\t" // Add c to the EAX register (performing b + c)
		"movl %%eax, %0   \n\t" // Move the result (now in EAX) into a
		: "=r"(c)				// Output operand: a (result)
		: "r"(a), "r"(b)		// Input operands: b and c
		: "%eax"				// Clobbered register: EAX
	);

	return c;
}

int asm_sub(int a, int b)
{
	int c = 0;

	__asm__ volatile (
		"movl %1, %%eax   \n\t" // Move b into the EAX register
		"subl %2, %%eax   \n\t" // Subtract c from the EAX register (performing b - c)
		"movl %%eax, %0   \n\t" // Move the result (now in EAX) into a
		: "=r"(c)				// Output operand: a (result)
		: "r"(a), "r"(b)		// Input operands: b and c
		: "%eax"				// Clobbered register: EAX
	);

	return c;
}

int asm_mul(int a, int b)
{
	int c = 0;

	__asm__ volatile (
		"movl %1, %%eax   \n\t" // Move b into the EAX register
		"imull %2, %%eax  \n\t" // Multiply EAX with c (performing b * c)
		"movl %%eax, %0   \n\t" // Move the result (now in EAX) into a
		: "=r"(c)				// Output operand: a (result)
		: "r"(a), "r"(b)		// Input operands: b and c
		: "%eax"				// Clobbered register: EAX
	);

	return c;
}

float asm_div(float a, float b)
{
	float c = 0;

	if(b == 0)
	{
		puts("[x] Division by 0! (b == 0)...Exiting....");
		exit(-1);
	}

	__asm__ volatile (
		"fld %1             \n\t" // Load b onto the floating-point stack
		"fdiv %2            \n\t" // Divide the top of the stack by c
		"fstp %0            \n\t" // Store the result from the stack into a
		: "=m"(c)				  // Output operand: a (result), memory constraint
		: "m"(a), "m"(b)		  // Input operands: b and c, memory constraint
		: "st"					  // Clobbered register: ST(0), the top of the stack
	);

	return c;
}