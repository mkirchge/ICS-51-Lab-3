/*
ICS 51. Lab #3

IMPORTATNT NOTES:

- Noramally, the compiler adds the prologue/epilogue code for the callee. However, when the
__declspec(naked) attribute is used, it is the programmer's responsibility to write the
prologue/epilogue code. In this lab you need to write prologue/epilogue code sections only
for the functions that have been declared with the naked attribute, namely:

+ findMinIndex
+ recFactorial
+ performOperation
+ fibonacci

- You cannot define any local variable in the C code. If you need additional variables in your
assembly code, you have to allocate space for them in the prologue of the function.

- You are not allowed to change anything in this file except for adding your assembly code
between the lines marked "BEGIN YOUR CODE HERE" and "END YOUR CODE HERE".

- Remember to fill in your name, student ID below.

- ONLY submit the lab3.c file for grading.

*/

char *yourName = "Maximillian Kirchgesner";
char *yourStudentID = "27238431";

/***********************************************************************************************

PART 1: Selection Sort

Selection sort is an in-place comparison sort algorithm that works by dividing the input list
into two parts: the sublist of items already sorted, which is built up from left to right of
the list, and the sublist of items remaining to be sorted that occupy the rest of the list.
Initially, the sorted sublist is empty and the unsorted sublist is the entire input list.
The algorithm proceeds by finding the smallest element in the unsorted sublist, exchanging it
with the leftmost unsorted element (putting it in sorted order), and moving the sublist
boundaries one element to the right. To learn more, read:
https://en.wikipedia.org/wiki/Selection_sort#Example

Our implementation here uses a function called "findMinIndex" to find the index of smallest
element between ith index and jth index of the array. The function "selectionSort" uses
this function to find the smallest number and exchanges it with the leftmost unsorted element
in each iteration. You need to implement the behavior of both functions in x86 assembly.

************************************************************************************************/

__declspec(naked)
int findMinIndex(int integer_array[], int i, int j)
{

	// C code to be converted to x86 assembly
	/*
	int iMin = i;
	// test against elements after i and before j to find the smallest
	for ( i ; i < j; i++) {
		// if this element is less, then it is the new minimum
		if (integer_array[i] < integer_array[iMin]) {
			// found new minimum; remember its index
			iMin = i;
		}
	}

	return iMin;
	*/

	__asm{
		// BEGIN YOUR CODE HERE
			// most of this is from example in discussion slides (proc_call_example)
			push ebp
			// now ebp points to top of stack
			mov ebp, esp
			push edi
			push esi
			mov eax, j
			mov ebx, i 
			mov ecx, integer_array
			mov edx, ebx

		FORLOOP1 : //for ( i ; i < j; i++)
			// if the element is greater than or equal, then jump to the end
			cmp ebx, eax
			jge FORLOOP1_END
			// otherwise, if it is les than, this is the new minimum
			mov esi, [ecx + 4 * ebx]
			mov edi, [ecx + 4 * edx]
			cmp esi, edi
			// if it is greater than or equal, then jump to LABEL
			jge LABEL
			// put the new minimum into edx
			mov edx, ebx

		LABEL :
			inc ebx
			jmp FORLOOP1

		FORLOOP1_END :
			// pop in reverse order they were pushed in
			mov ecx, edx // put back into the integer_array
			pop esi
			pop edi
			// have to move ebp back into esp
			mov esp, ebp
			pop ebp
		// END YOUR CODE HERE
			ret
	}

}

void selectionSort(int integer_array[], int array_size)
{

	// C code to be converted to x86 assembly
	/*
	int j;
	int iMin;
	int temp;

	// advance the position through the entire array //
	// (could do j < n-1 because single element is also min element) //

	for (j = 0; j < array_size-1; j++) {

		// find the index of min element in the unsorted a[j .. n-1] //
		iMin = findMinIndex (integer_array, j, array_size);

		if(iMin != j) { // swap values
			temp = integer_array[iMin];
			integer_array[iMin] = integer_array [j];
			integer_array[j] = temp;
		}
	}
	*/

	__asm{
		// BEGIN YOUR CODE HERE
			// a lot of this is taken from discussion slides (proc_call_example)
			mov eax, [ebp + 12] // array_size
			mov esi, [ebp + 8] // integer_array
			// counter
			mov edi, 0
			// subtract 1 from array size so we can run for loop using it (up to: array_size - 1)
			sub eax, 1

		FORLOOP1: // for (j = 0; j < array_size-1; j++)
			cmp eax, edi
			jl FORLOOP1_END
			mov ebx, [ebp + 12]
			push ebx
			push edi
			push esi
			// call the findMinIndex function written above
			call findMinIndex
			add esp, 12
			mov ebx, edx
			cmp edi, ebx
			je LABEL
			//temp = integer_array[iMin]
			mov edx, [esi + 4 * ebx]
			mov ecx, [esi + 4 * edi]
			xor edx, ecx
			xor ecx, edx
			xor edx, ecx
			//integer_array[iMin] = integer_array[j]
			//integer_array[j] = temp
			mov[esi + 4 * ebx], edx
			mov[esi + 4 * edi], ecx

		LABEL :
			inc edi
			jmp FORLOOP1

		FORLOOP1_END :
			// put esi back into the integer_array
			mov[ebp + 8], esi
		// END YOUR CODE HERE
	}

}

/***********************************************************************************************

PART 2: Recursive Factorial

Implement two functions that calculates the factorial of an integer not greater than 12.

The 1st function, factorial (unsigned int input, unsigned int *output), is the non
recursive wrapper function which takes the input, sets up the stack, calls the recursive
function and finally puts the result in the location pointed to by the pointer argument called
output.

The 2nd function, recFactorial(unsigned int n), is the recursive function that actually
calculates the factorial.

Given an unsigned integer number n,we define FACTORIAL(n) as:

FACTORIAL(n) =  1                   ,    if n = 0 or n = 1
n * FACTORIAL(n-1)  ,    if n > 1

Tip:

- You need to use IMUL instruction to multiply unsigned integers. Learn how to use it:
http://www.cs.virginia.edu/~evans/cs216/guides/x86.html#instructions

************************************************************************************************/

__declspec(naked)
unsigned int recFactorial(unsigned int n)
{

	// C code to be converted to x86 assembly
	/*
	if (n<=1)
		return 1;
	else
		return n * recFactorial(n-1);
	*/

	__asm{
		// BEGIN YOUR CODE HERE
			push ebp
			mov ebp, esp
			mov ecx, n // gets the parameter
			// if (n <= 1)
			cmp ecx, 1
			jg ELSELOOP1
			// if it is less than or equal to 1, then return 1 (put 1 into ebx)
			mov eax, 1
			jmp THE_END_OF_RECURSION

		ELSELOOP1 :
			dec ecx
			push ecx
			// Recursively call this function (recFactorial)
			call recFactorial
			add esp, 4
			mov ecx, eax
			imul ecx, n
			mov eax, ecx

		THE_END_OF_RECURSION :
			// pop in reverse order they were pushed in
			// have to move ebp back into esp
			mov esp, ebp
			pop ebp
		// END YOUR CODE HERE
			ret
	}

}

void factorial(unsigned int input, unsigned int *output)
{

	// C code to be converted to x86 assembly
	/*
	*output = recFactorial (input);
	*/

	__asm{
		// BEGIN YOUR CODE HERE
			// put the input parameter into register eax
			mov eax, [ebp + 8]
			push eax
			call recFactorial
			add esp, 4
			mov edx, [ebp + 12]
			mov[edx], eax
		// END YOUR CODE HERE
	}

}

/***********************************************************************************************

PART 3: Vector Operation

The vectorOperations function gets as parameters 4 arrays of the same length and
the number of elements in those arrays.

For each pair of elements in first_array and second_array, it calls performOperation
and stores the returned result in the same index of output_array.

output_array[index] = performOperation(operation_array[index], first_array[index], second_array[index]).

You also need to implement the body of performOperation function according to the provided
C code.

************************************************************************************************/

__declspec(naked)
int performOperation(int operation, int first, int second)
{

	// C code to be converted to x86 assembly
	/*
	if (operation==0)
		return first + second;
	else if (operation==1)
		return first - second;
	else if (operation==2)
		return (first > second)? first : second;
	else if (operation==3)
		return (first < second)? first : second;
	else
		return first;
	*/

	__asm {
		// BEGIN YOUR CODE HERE
			push ebp
			mov ebp, esp
			mov edx, operation // gets the first parameter
			mov ebx, second // gets the third parameter
			// if (operation==0)
			cmp edx, 0
			jne FIRST_IF_STATEMENT
			// then add first + second
			add first, ebx
			mov eax, first
			jmp THE_END

		FIRST_IF_STATEMENT :
			// else if (operation==1)
			cmp edx, 1
			jne SECOND_IF_STATEMENT
			// return first - second
			sub first, ebx
			mov eax, first
			jmp THE_END

		SECOND_IF_STATEMENT :
			// else if (operation==2)
			cmp edx, 2
			jne THIRD_IF_STATEMENT
			cmp ebx, first
			jg IF_NOT_GREATER
			mov eax, first
			jmp THE_END

		IF_NOT_GREATER :
			mov eax, ebx
			jmp THE_END

		THIRD_IF_STATEMENT :
			// else if (operation==3)
			cmp edx, 3
			jne ELSE_STATEMENT
			cmp ebx, first
			jl IF_GREATER
			mov eax, first
			jmp THE_END

		IF_GREATER :
			mov eax, ebx
			jmp THE_END

		ELSE_STATEMENT :
			mov eax, first

		THE_END :
			mov esp, ebp
			pop ebp
		// END YOUR CODE HERE
			ret
	}

}

void vectorOperations(int number_of_elements, int *first_array, int *second_array,
	int *operation_array, int *output_array)
{
	// C code to be converted to x86 assembly
	/*
	int i;
	for (i=0; i<number_of_elements; i++) {
		output_array[i] = performOperation(operation_array[i], first_array[i], second_array[i]);
	}
	*/

	__asm {
		// BEGIN YOUR CODE HERE
			mov esi, [ebp + 24]
			//counter (int i)
			mov edi, 0

		FORLOOP1: // for (i=0; i<number_of_elements; i++)
			cmp[ebp + 8], edi
			// if we have reached the end of the output_array, then jump to the end (FORLOOP1_END)
			jl FORLOOP1_END
			// if i is less than number_of_elements (i<number_of_elements)
			push eax
			push ecx
			mov ebx, [ebp + 12]
			mov eax, [ebp + 16]
			mov ecx, [ebp + 20]
			mov ebx, [ebx + 4 * edi]
			mov eax, [eax + 4 * edi]
			mov ecx, [ecx + 4 * edi]
			push eax
			push ebx
			push ecx
			// performOperation(operation_array[i], first_array[i], second_array[i]);
			call performOperation
			add esp, 12
			mov[esi + 4 * edi], eax
			inc edi			
			// pop in reverse order they were pushed
			pop ecx
			pop eax
			jmp FORLOOP1

		FORLOOP1_END :
			// put edi back into output_array
			mov[ebp + 24], esi
		// END YOUR CODE HERE
	}
}

/***********************************************************************************************

PART 4: Change Letter Case

You are given an array of characters (a string) that may contain numbers, upper and lower case
letters, punctuations symbols, etc. Implement a function to convert all uppercase letters to
lowercase and vice versa.
For example "ThisIsTheFinalLabOf51" should be converted to "tHISiStHEfINALlABoF51".
The function has the following argument: An array of charaters and the size of the array. The
array should be changed in place. There is no explicit output for this function.

Tips:

- Note that each element of the arrays is a char (1 byte). You can NOT use a 32-bit register
to read/write from/to a memory location. Instead you should use the 8-bit version of the x86
general-purpose registers.
More info: http://www.cs.virginia.edu/~evans/cs216/guides/x86.html#registers

- The link to the ascii codes is here: http://www.asciitable.com/

************************************************************************************************/

void changeCase(char char_array[], int array_size) {

	__asm{
		// BEGIN YOUR CODE HERE
			// put the array into a register
			mov edx, [ebp + 8] // gets the first parameter (char_array[])
			mov ecx, [ebp + 12] // gets the second parameter (array_size)

		LOOP_THROUGH_STRING:
			// see if the we have reached the end of the string by using or on the register holding array_size
			cmp ecx, 0
			// if we have reached the end of the array, then jump to the end loop
			jz LOOP1_END
			// we use 8-bit version (byte ptr) instead of 32-bit (dword ptr - since each element is 1 byte)
			mov al, byte ptr[edx]
			// We must check if the current letter is uppercase (A or Z)
			cmp al, 'A'
			jl LABEL
			cmp al, 'Z'
			jle CONVERT_TO_LOWERCASE
			// Then check if the current letter is lowercase (a or z)
			cmp al, 'a'
			jl LABEL
			cmp al, 'z'
			jle CONVERT_TO_UPPERCASE
			jmp LABEL

		CONVERT_TO_LOWERCASE :
			or al, 0x20
			mov byte ptr[edx], al
			jmp LABEL

		CONVERT_TO_UPPERCASE :
			and al, 0xdf
			mov[edx], al
			jmp LABEL

		LABEL :
			inc edx
			dec ecx
			jmp LOOP_THROUGH_STRING

		LOOP1_END :
			// END YOUR CODE HERE
	}
}

/***********************************************************************************************

PART 5: Fibonacci

Implement a recursive function in assembly which returns the Nth number in the fibbonaci
series (1, 1, 2, 3, 5, 8, ...).

************************************************************************************************/

__declspec(naked)
unsigned int fibonacci(unsigned int n) {

	// C code to be converted to x86 assembly
	/*
	if (n < 2)
		return 1;
	else
		return fibonacci(n - 1) + fibonacci(n - 2);
	*/

	__asm{
		// BEGIN YOUR CODE HERE
			push ebp
			mov ebp, esp
			push edi
			push esi
			push ebx
			// get the parameter (int n)
			mov ebx, n 
			// if the number is less than 2, then continue
			cmp ebx, 1
			// if it not less than 2, then jump to else statement
			jg ELSE_STATEMENT
			mov eax, 1
			jmp THE_END_OF_RECURSION

		ELSE_STATEMENT:
			push eax
			push ecx
			mov ecx, ebx
			dec ecx
			push ecx
			// call the fibonacci function recursively
			call fibonacci
			//fibonacci(n - 1)
			mov ecx, ebx
			mov edi, eax
			add esp, 4
			sub ecx, 2
			push ecx
			// call the fibonacci function recursively
			call fibonacci
			//fibonacci(n - 2)
			mov esi, eax
			add esp, 4
			// pop in reverse order they were pushed in
			pop ecx
			pop eax
			// now add the fib(n-1) + fib(n-2)
			mov eax, edi
			add eax, esi

		THE_END_OF_RECURSION:
			// pop in reverse order they were pushed in
			pop ebx
			pop esi
			pop edi
			mov esp, ebp
			pop ebp
			// END YOUR CODE HERE
			ret
	}
}