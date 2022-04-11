#include <stdio.h>  // Used for printf()
#include <stdlib.h> // Used for exit()


/***********************
        GLOBALS
***********************/


// DO NOT TOUCH THESE
#define TRUE 1
#define FALSE 0
#define COMMAND_LENGTH 3

// You can edit these
#define MEMORY_SIZE 25                     // Number of memory blocks
#define MAX_NUMBER_OF_INSTRUCTIONS 2000
#define MAX_ITERATIONS 500000

enum { INVALID_COMMAND=-1, moo=0, mOo, moO, mOO, Moo, MOo, MoO, MOO, OOO, MMM, OOM, oom };

short memoryBlocksArray[MEMORY_SIZE];
short currentBlockIndex = 0;

// Register, necessary for instruction MMM
short reg = 0;
short isRegisterInitialized = FALSE;


/***********************
         FUNCTIONS
***********************/


// Returns true if the two command strings are equal
// C does not have built-in string comparison (it's in string.h)
// Assumes strings have the same length
short commandNamesEqual(char *a, char *b)
{
    int i=0;

    for(i = 0; i < COMMAND_LENGTH; i++)
    {
        if(a[i] != b[i])
            return FALSE;
    }

    return TRUE;
}

void exitWithError(char *command, char *message)
{
    printf("\nError [%s]: %s\n", command, message);
    // TODO: should find a way to exit from main function, both for clarity and because this causes errors if there's an opened file
    exit(EXIT_FAILURE);
}

// Takes a command (instruction) name and returns its instruction code
// If the command does not exist, returns INVALID_COMMAND
short getCommandCode(char *commandName)
{
    if(commandNamesEqual(commandName, "moo")) { return moo; }
    else if(commandNamesEqual(commandName, "mOo")) { return mOo; }
    else if(commandNamesEqual(commandName, "moO")) { return moO; }
    else if(commandNamesEqual(commandName, "mOO")) { return mOO; }
    else if(commandNamesEqual(commandName, "Moo")) { return Moo; }
    else if(commandNamesEqual(commandName, "MOo")) { return MOo; }
    else if(commandNamesEqual(commandName, "MoO")) { return MoO; }
    else if(commandNamesEqual(commandName, "MOO")) { return MOO; }
    else if(commandNamesEqual(commandName, "OOO")) { return OOO; }
    else if(commandNamesEqual(commandName, "MMM")) { return MMM; }
    else if(commandNamesEqual(commandName, "OOM")) { return OOM; }
    else if(commandNamesEqual(commandName, "oom")) { return oom; }
    
    return INVALID_COMMAND;
}

// Executes the command which corresponds to the given instruction code.
// Returns index of the next instruction to execute.
// The first parameter is necessary for the mOO command,
// the other parameters are necessary for MOO and moo commands.
short execCommand(short commandCode, short *instructionsArray, short instructionIndex, short numberOfInstructions)
{
    // These allow to ignore nested moo/MOO when finding loop start/end
    // In the following example, the first 'MOO' should match only the last 'moo'
    // while the last 'moo' should match only the outer 'MOO'
    // MOO ... MOO ... moo ... moo
    short MOO_count = 0;
    short moo_count = 0;

    switch (commandCode)
    {
        case 0:
        
            // moo
            // This command is connected to the MOO command. When encountered during normal execution,
            // it searches the program code in reverse looking for a matching MOO command
            // and begins executing again starting from the found MOO command.
            // When searching, it skips the instruction that is immediately before it (see MOO).
            
            instructionIndex -= 2;  // Skip previous instruction
            moo_count = 0;

            while (instructionIndex >= 0)
            {
                switch (instructionsArray[instructionIndex])
                {
                    case moo:
                        moo_count++;
                        break;

                    case MOO:
                        if(moo_count == 0)
                            return instructionIndex;
                        else
                            moo_count--;
                        break;
                }

                instructionIndex--;
            }
            
            exitWithError("MOO", "could not find a matching 'MOO' command");
            break;

        case 1:
        
            // mOo
            // Moves current memory position back one block

            if(currentBlockIndex > 0) {
                currentBlockIndex--;
            } else {
                exitWithError("mOo", "trying to access a memory block before the first one.");
            }
            break;

        case 2:

            // moO
            // Moves current memory position forward one block

            if(currentBlockIndex < MEMORY_SIZE - 1) {
                currentBlockIndex++;
            } else {
                exitWithError("moO", "not enough memory.");
            }
            break;

        case 3:
        
            // mOO
            // Execute value in current memory block as if it were an instruction.
            // The command executed is based on the instruction code value (see https://bigzaphod.github.io/COW/)
            // (for example, if the current memory block contains a 2, then the moO command is executed).
            // An invalid command exits the running program.
            // Cannot call itself, as it would cause an infinite loop.
            
            if(memoryBlocksArray[currentBlockIndex] == commandCode) {
                exitWithError("mOO", "cannot call itself, it would cause an infinite loop.");
            }
            else {
                // If the current block contains an executable command
                // TODO it would be better to have a function to check this
                if(memoryBlocksArray[currentBlockIndex] >= 0 && memoryBlocksArray[currentBlockIndex] <= 11) {
                    execCommand(memoryBlocksArray[currentBlockIndex], instructionsArray, instructionIndex, instructionIndex);
                } else {
                    /* TODO: from "mOO" description is not clear if the program should exit correctly or with an arror.
                    Since there is no other way to exit, it shouldn't be an error I guess (only if the function is called because of mOO) */
                    printf("[mOO]: quit program");
                    exit(EXIT_SUCCESS);
                }
            }
            break;

        case 4:

            // Moo
            // If current memory block has a 0 in it, read a single ASCII character from STDIN and store it in the current memory block.
            // If the current memory block is not 0, then print the ASCII character that corresponds to the value in the current memory block to STDOUT.

            if(memoryBlocksArray[currentBlockIndex] == 0) {
                exitWithError("Moo", "not implemented yet");
            } else {
                if(memoryBlocksArray[currentBlockIndex] > 0 && memoryBlocksArray[currentBlockIndex] < 256) {
                    printf("%c", memoryBlocksArray[currentBlockIndex]);
                    //printf("%c -- %d\n", memoryBlocksArray[currentBlockIndex], memoryBlocksArray[currentBlockIndex]);
                } else {
                    exitWithError("Moo", "this character cannot be printed to STDOUT.");
                }
            }
            break;

        case 5:
            
            // MOo
            // Decrement current block value by 1

            memoryBlocksArray[currentBlockIndex]--;
            break;

        case 6:
            
            // MoO
            // Increment current block value by 1

            memoryBlocksArray[currentBlockIndex]++;
            break;

        case 7:
        
            // MOO
            // If current memory block value is 0, skip next command and resume execution *after* the next matching moo command.
            // If current memory block value is not 0, then continue with next command.
            // Note that the fact that it skips the command immediately following it has interesting ramifications for where the matching moo command really is.
            // For example, the following will match the second and not the first moo: OOO MOO moo moo

            if(memoryBlocksArray[currentBlockIndex] == 0)
            {
                MOO_count = 0;

                instructionIndex += 2;  // Skip next instruction

                while (instructionIndex < numberOfInstructions)
                {
                    switch (instructionsArray[instructionIndex])
                    {
                        case moo:
                            if(MOO_count == 0)
                                return instructionIndex + 1;
                            else
                                MOO_count--;
                            break;
                        
                        case MOO:
                            MOO_count++;
                            break;
                    }

                    instructionIndex++;
                }
                
                exitWithError("MOO", "could not find a matching 'moo' command");
            }
            // else do nothing
            break;

        case 8:
            
            // OOO
            // Set current memory block value to zero
            
            memoryBlocksArray[currentBlockIndex] = 0;
            break;

        case 9:
        
            // MMM
            // If no current value in register, copy current memory block value.
            // If there is a value in the register, then paste that value into the current memory block and clear the register.
            
            if(!isRegisterInitialized) {
                reg = memoryBlocksArray[currentBlockIndex];
                isRegisterInitialized = TRUE;
            } else {
                memoryBlocksArray[currentBlockIndex] = reg;
                isRegisterInitialized = FALSE;
            }
            break;

        case 10:
            
            // OOM
            // Print value of current memory block to STDOUT as an integer

            // Added the newline (\n) because the 99bottles and Fibonacci examples assume this,
            // even though the language specification doesn't say anything about it
            printf("%d\n", memoryBlocksArray[currentBlockIndex]);
            break;

        case 11:
        
            // oom
            // Read an integer from STDIN and put it into the current memory block

            exitWithError("oom", "not implemented yet");
            break;

        default:
            exitWithError("execCommand()", "invalid command code.");
    }

    // If MOO or moo did not return, go to the next instruction
    return instructionIndex + 1;
}

// Reads a string containing the source code of a program, finds
// the instruction code (opcode) of every command and puts it into an array.
// Returns the number of commands in the program
// (assuming there's a command per line, returns the number of lines of code)
short parser(char *sourceCode, short *opcodesArray, short opcodesArrayLength)
{
    short numberOfInstructions = 0;
    short suitableCharCount = 0;
    short commandCode = INVALID_COMMAND;

    char commandName[COMMAND_LENGTH + 1];
    commandName[COMMAND_LENGTH - 1] = '\0';

    short i = 0;

    // Read source code to find commands
    for(i = 0; sourceCode[i] != '\0'; i++)
    {
        // Check if the current character can be part of a command
        switch(sourceCode[i])
        {
            case 'm':
            case 'M':
            case 'o':
            case 'O':
                suitableCharCount++;
                break;
            default:
                // As per specification, any character that is not part of a command is ignored
                suitableCharCount = 0;
                // Skip to next character
                continue;
        }

        // This works because every COW instruction is exactly 3 characters long
        if(suitableCharCount == COMMAND_LENGTH)
        {
            // Turn the three consecutive suitable characters into a string
            commandName[0] = sourceCode[i-2];
            commandName[1] = sourceCode[i-1];
            commandName[2] = sourceCode[i];

            commandCode = getCommandCode(commandName);

            // If the found string is a valid COW command
            if(commandCode != INVALID_COMMAND)
            {
                suitableCharCount = 0;
                
                //printf("Found command: %s (%d)\n", commandName, commandCode);
                
                if(numberOfInstructions < opcodesArrayLength)
                {
                    opcodesArray[numberOfInstructions] = commandCode;
                    numberOfInstructions++;
                }
                else {
                    exitWithError("parser", "too many instructions, not enough space in opcodes array");
                }
            }
            else
            {
                // If source code is "mmoo" -> "mmo" is not a valid command, but "moo" is.
                // This line allows to check the command validity again if the next character is suitable
                suitableCharCount--;
            }
        }
    }

    return numberOfInstructions;
}

short parserFile(char *fileName, short *opcodesArray, short opcodesArrayLength)
{
    short numberOfInstructions = 0;
    short suitableCharCount = 0;
    short commandCode = INVALID_COMMAND;

    char commandName[COMMAND_LENGTH + 1];
    commandName[COMMAND_LENGTH - 1] = '\0';

    int c;

    // https://stackoverflow.com/a/3463793
    FILE *file = fopen(fileName, "r");

    if(!file)
    {
        exitWithError("parserfile", "could not open file for reading");
    }

    // Read source code to find commands
    while ((c = getc(file)) != EOF)
    {
        // Check if the current character can be part of a command
        switch(c)
        {
            case 'm':
            case 'M':
            case 'o':
            case 'O':
                suitableCharCount++;
                break;
            default:
                // As per specification, any character that is not part of a command is ignored
                suitableCharCount = 0;
                break;
        }

        // TODO Looks inefficient...
        commandName[0] = commandName[1];
        commandName[1] = commandName[2];
        commandName[2] = c;

        // This works because every COW instruction is exactly 3 characters long
        if(suitableCharCount == COMMAND_LENGTH)
        {
            commandCode = getCommandCode(commandName);

            // If the found string is a valid COW command
            if(commandCode != INVALID_COMMAND)
            {
                suitableCharCount = 0;

                //printf("Found command: %s (%d)\n", commandName, commandCode);
                
                if(numberOfInstructions < opcodesArrayLength)
                {
                    opcodesArray[numberOfInstructions] = commandCode;
                    numberOfInstructions++;
                }
                else {
                    // TODO exit() docs state you have to close files before exiting
                    fclose(file);
                    exitWithError("parser", "too many instructions, not enough space in opcodes array");
                }
            }
            else
            {
                // If source code is "mmoo" -> "mmo" is not a valid command, but "moo" is.
                // This line allows to check the command validity again if the next character is suitable
                suitableCharCount--;
            }
        }
    }

    fclose(file);

    return numberOfInstructions;
}

void printMemory(short memoryArray[], int arrayLength)
{
    int i = 0;

    printf("Memory looks like this:\n");
    for(i = 0; i < arrayLength; i++)
    {
        printf("| %d ", memoryArray[i]);
    }
    printf("|");

    printf("\n\nIn ASCII:\n");
    for(i = 0; i < arrayLength; i++)
    {
        switch (memoryArray[i])
        {
            case 0:
                printf("|   ");
                break;
            case 10:
                printf("| LF ");
                break;
            case 32:
                printf("| space ");
                break;
            default:
                printf("| %c ", memoryArray[i]);
                break;
        }
    }
    printf("|");
}


/***********************
         MAIN
***********************/


int main(int argc, char *argv[])
{
    char *fileName;

    if(argc == 2)
    {
        fileName = argv[1];
    }
    else
    {
        exitWithError("main", "you should pass exactly one argument, the name of the source code file");
    }
    
    int i = 0;
    
    // Initialize memory to zeros
    for (i = 0; i < MEMORY_SIZE; i++)
    {
        memoryBlocksArray[i] = 0;
    }
    
    // PARSE PROGRAM

    printf("\nStarting parser.\n");
    short opcodesArray[MAX_NUMBER_OF_INSTRUCTIONS];
    short numberOfInstructions = numberOfInstructions = parserFile(fileName, opcodesArray, MAX_NUMBER_OF_INSTRUCTIONS);
    //short numberOfInstructions = parser(sourceCode, opcodesArray, MAX_NUMBER_OF_INSTRUCTIONS);
    printf("Reached end of source code.\n");
    
    if(numberOfInstructions > 0)
    {
        // EXECUTE PROGRAM

        printf("\nExecuting program.\n");
        printf("Number of memory blocks: %d\n", MEMORY_SIZE);
        printf("Index of current block: %d\n", currentBlockIndex);
        printf("Output: ");

        unsigned int iterations = 0;
        i = 0;

        while(i < numberOfInstructions)
        {
            i = execCommand(opcodesArray[i], opcodesArray, i, numberOfInstructions);

            iterations++;

            if(iterations >= MAX_ITERATIONS) {
                exitWithError("runner", "program is taking too long, infinite loop?");
            }
        }

        printf("\nProgram end.\n\n");

        printf("Number of commands in program: %d\n", numberOfInstructions);
        printf("Number of executed commands: %u\n", iterations);
    }
    else
    {
        printf("No valid commands found.\n");
    }

    printf("\n");
    printMemory(memoryBlocksArray, MEMORY_SIZE);

    return 0;
}
