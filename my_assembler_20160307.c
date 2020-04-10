/*
 * ȭ�ϸ� : my_assembler_00000000.c 
 * ��  �� : �� ���α׷��� SIC/XE �ӽ��� ���� ������ Assembler ���α׷��� ���η�ƾ����,
 * �Էµ� ������ �ڵ� ��, ��ɾ �ش��ϴ� OPCODE�� ã�� ����Ѵ�.
 * ���� ������ ���Ǵ� ���ڿ� "00000000"���� �ڽ��� �й��� �����Ѵ�.
 */

/*
 *
 * ���α׷��� ����� �����Ѵ�. 
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
// #define DEBUG 1
static int debug_count;
// ���ϸ��� "00000000"�� �ڽ��� �й����� ������ ��.
#include "my_assembler_20160307.h"
void debug_print();

/* ----------------------------------------------------------------------------------
 * ���� : ����ڷ� ���� ����� ������ �޾Ƽ� ��ɾ��� OPCODE�� ã�� ����Ѵ�.
 * �Ű� : ���� ����, ����� ���� 
 * ��ȯ : ���� = 0, ���� = < 0 
 * ���� : ���� ����� ���α׷��� ����Ʈ ������ �����ϴ� ��ƾ�� ������ �ʾҴ�. 
 *		   ���� �߰������� �������� �ʴ´�. 
 * ----------------------------------------------------------------------------------
 */
int main(int args, char *arg[])
{
	init_inst_file("inst.data");
	init_input_file("input.txt"); // Debug
	printf("Input file Line number = %d\n", line_num);
	// for (int i = 0; i < MAX_LINES; i++)
	// 	printf("%s\n", input_data[i]);
	assem_pass1();
	debug_print();
	printf("DEbug count = %d ", debug_count);
	make_opcode_output("output.txt");
	// if (init_my_assembler() < 0)
	// {
	// 	printf("init_my_assembler: ���α׷� �ʱ�ȭ�� ���� �߽��ϴ�.\n");
	// 	return -1;
	// }

	// if (assem_pass1() < 0)
	// {
	// 	printf("assem_pass1: �н�1 �������� �����Ͽ����ϴ�.  \n");
	// 	return -1;
	// }
	// make_opcode_output("output_00000000");

	/*
	* ���� ������Ʈ���� ���Ǵ� �κ�
	
	make_symtab_output("symtab_00000000");
	if(assem_pass2() < 0 ){
		printf(" assem_pass2: �н�2 �������� �����Ͽ����ϴ�.  \n") ; 
		return -1 ; 
	}

	make_objectcode_output("output_00000000") ; 
	*/
	return 0;
}

/* ----------------------------------------------------------------------------------
 * ���� : ���α׷� �ʱ�ȭ�� ���� �ڷᱸ�� ���� �� ������ �д� �Լ��̴�. 
 * �Ű� : ����
 * ��ȯ : �������� = 0 , ���� �߻� = -1
 * ���� : ������ ��ɾ� ���̺��� ���ο� �������� �ʰ� ������ �����ϰ� �ϱ� 
 *		   ���ؼ� ���� ������ �����Ͽ� ���α׷� �ʱ�ȭ�� ���� ������ �о� �� �� �ֵ���
 *		   �����Ͽ���. 
 * ----------------------------------------------------------------------------------
 */
int init_my_assembler(void)
{
	int result;

	if ((result = init_inst_file("inst.data")) < 0)
		return -1;
	if ((result = init_input_file("input.txt")) < 0)
		return -1;
	return result;
}

/* ----------------------------------------------------------------------------------
 * ���� : �ӽ��� ���� ��� �ڵ��� ������ �о� ���� ��� ���̺�(inst_table)�� 
 *        �����ϴ� �Լ��̴�. 
 * �Ű� : ���� ��� ����
 * ��ȯ : �������� = 0 , ���� < 0 
 * ���� : ���� ������� ������ �����Ӱ� �����Ѵ�. ���ô� ������ ����.
 *	
 *	===============================================================================
 *		   | �̸� | ���� | ���� �ڵ� | ���۷����� ���� | NULL|
 *	===============================================================================	   
 *		
 * ----------------------------------------------------------------------------------
 */

//inst �޸� �Ҵ� �Լ� . ��ū ���� ����
void memory_allocation(char **parameter, char **inst_token)
{
	*parameter = (char *)malloc(strlen(*inst_token));
	strcpy(*parameter, *inst_token);
	*inst_token = strtok(NULL, "\t");
}
int init_inst_file(char *inst_file)
{
	FILE *file;
	int errno;
	char buffer[100] = {
		-1,
	};
	char *inst_token = NULL;

	file = fopen(inst_file, "r");
	if (file == NULL)
		return -1;
	for (int i = 0; i < MAX_INST; i++)
	{
		fgets(buffer, sizeof(buffer), file);
		inst_table[i] = (inst *)malloc(sizeof(inst));
		inst_token = strtok(buffer, "\t");
		if (feof(file))
			break;
		while (inst_token != NULL)
		{
			//mnemonic �� ��ū ����
			memory_allocation(&(inst_table[i]->mnemonic), &inst_token);

			//operand_count �� ��ū ����
			inst_table[i]->operand_count = atoi(inst_token);
			inst_token = strtok(NULL, "\t");

			//format �� ��ū ����
			memory_allocation(&(inst_table[i]->format), &inst_token);

			//opcode �� ��ū ����
			inst_token[strlen(inst_token) - 1] = '\0'; //���๮�� ����
			memory_allocation(&(inst_table[i]->opcode), &inst_token);

#ifdef DEBUG
			printf("%s	", inst_table[i]->mnemonic);
			printf("%d	", inst_table[i]->operand_count);
			printf("%s	", inst_table[i]->format);
			printf("%s	\n", inst_table[i]->opcode);

#endif

			inst_index++;
		}
	}
	fclose(file);
	return errno;
}

/* ----------------------------------------------------------------------------------
 * ���� : ����� �� �ҽ��ڵ带 �о� �ҽ��ڵ� ���̺�(input_data)�� �����ϴ� �Լ��̴�. 
 * �Ű� : ������� �ҽ����ϸ�
 * ��ȯ : �������� = 0 , ���� < 0  
 * ���� : ���δ����� �����Ѵ�.
 *		
 * ----------------------------------------------------------------------------------
 */
int init_input_file(char *input_file)
{
	FILE *file;
	int errno;
	char buffer[256] = {
		-1,
	};
	/* add your code here */
	file = fopen(input_file, "r");
	if (file == NULL)
		return -1;

	for (int i = 0; i < MAX_LINES; i++)
	{
		if (feof(file))
			break;
		fgets(buffer, sizeof(buffer), file);
		input_data[i] = (char *)malloc(sizeof(buffer));
		//���๮�� ������
		buffer[strlen(buffer) - 1] = '\0';
		strcpy(input_data[i], buffer);
		line_num++;

#ifdef DEBUG
		printf("%s\n", input_data[i]);
#endif
	}
	fclose(file);
	return errno;
}

/* ----------------------------------------------------------------------------------
 * ���� : �ҽ� �ڵ带 �о�� ��ū������ �м��ϰ� ��ū ���̺��� �ۼ��ϴ� �Լ��̴�. 
 *        �н� 1�� ���� ȣ��ȴ�. 
 * �Ű� : �Ľ��� ���ϴ� ���ڿ�  
 * ��ȯ : �������� = 0 , ���� < 0 
 * ���� : my_assembler ���α׷������� ���δ����� ��ū �� ������Ʈ ������ �ϰ� �ִ�. 
 * ----------------------------------------------------------------------------------
 */
void clear_token_table(token *token_table)
{
	token_table->label = NULL;
	token_table->operator= NULL;
	token_table->comment = NULL;
	for (int i = 0; i < MAX_OPERAND; i++)
		token_table->operand[i] = NULL;
}

//������, �� ���� ����
void debug_print()
{
	for (int i = 0; i < token_line; i++)
	{
		debug_count++;
		// if(token_table[token_line]->label){
		printf("%s\t", token_table[i]->label);
		printf("%s\t", token_table[i]->operator);
		for (int k = 0; k < 3; k++)
		{
			if (token_table[i]->operand[k])
			{
				if (k == 0)
					printf("%s", token_table[i]->operand[k]);
				else
					printf(",%s", token_table[i]->operand[k]);
			}
			else
			{
				printf("\t");
				break;
			}
		}
		if ((token_table[i]->comment) == NULL)
			printf("\n");
		else
			printf("%s\n", token_table[i]->comment);
	}
	return;
}
int token_parsing(char *str)
{
	/* add your code here */
	char *parsing_token = NULL;
	char *sub_parsing_token = NULL; //�������� operand �� ������� ����� ��ū
	int islabel = 0;				// 0 : label ����, 1 : label ����
	char str_buffer[100] = {
		0,
	};

	//��ū �ʱ�ȭ
	token_table[token_line] = (token *)malloc(sizeof(token));
	clear_token_table(token_table[token_line]);
	//str �� str_buffer �� ����.
	strcpy(str_buffer, str);

	//���� ���� �˻�
	if (str_buffer[0] != '\t')
		islabel = 1;
	parsing_token = strtok(str_buffer, "\t");

	//"." , "\n"���� �����ϴ� �ּ� ����. �ʿ� �����Ƿ� �ٷ� �޸� ����
	if ((str_buffer[0] == '.') || (str_buffer[0] == '\n'))
	{
		free(token_table[token_line]);
		return 0;
	}
	while (parsing_token != NULL)
	{
		//Label �� �����ϴ� input line
		if (islabel == 1)
		{
			memory_allocation(&(token_table[token_line]->label), &parsing_token);
		}
		//Ư�� : RSUB �� ���� ���� ó��
		if (!(strcmp(parsing_token, "RSUB")))
		{
			memory_allocation(&(token_table[token_line]->operator), &parsing_token);
			memory_allocation(&(token_table[token_line]->comment), &parsing_token);
			token_line++;
			return 0;
		}
		//Operator ���� �Ҵ�
		memory_allocation(&(token_table[token_line]->operator), &parsing_token);
		if (parsing_token == NULL)
		{
			token_line++;
			return 0;
		}

		//Operand ���ڰ� ������ �� ��� ","�� ���еǴ� sub ��ū
		if (strchr(parsing_token, ','))
		{
			int operand_index = 0;
			sub_parsing_token = strtok(parsing_token, ",");
			while (sub_parsing_token != NULL)
			{

				token_table[token_line]->operand[operand_index] = (char *)malloc(strlen(sub_parsing_token));
				strcpy(token_table[token_line]->operand[operand_index++], sub_parsing_token);
				sub_parsing_token = strtok(NULL, ",");
			}
			//comment ���� ���� ��� ����
			if (!(parsing_token = strtok(NULL, "\t")))
			{
				token_line++;
				return 0;
			}
		}
		//operand���ڰ� 1�� �� ���
		else
		{
			memory_allocation(&(token_table[token_line]->operand[0]), &parsing_token);
			//comment ���� ���� ��� ����
			if (parsing_token == NULL)
			{
				token_line++;
				return 0;
			}
		}

		//comment �κ� ���� �Ҵ�
		memory_allocation(&(token_table[token_line]->comment), &parsing_token);
		token_line++;
	}

	return 0;
}

/* ----------------------------------------------------------------------------------
 * ���� : �Է� ���ڿ��� ���� �ڵ������� �˻��ϴ� �Լ��̴�. 
 * �Ű� : ��ū ������ ���е� ���ڿ� 
 * ��ȯ : �������� = ���� ���̺� �ε���, ���� < 0 
 * ���� : 
 *		
 * ----------------------------------------------------------------------------------
 */
int search_opcode(char *str)
{
	/* add your code here */
}

/* ----------------------------------------------------------------------------------
* ���� : ����� �ڵ带 ���� �н�1������ �����ϴ� �Լ��̴�.
*		   �н�1������..
*		   1. ���α׷� �ҽ��� ��ĵ�Ͽ� �ش��ϴ� ��ū������ �и��Ͽ� ���α׷� ���κ� ��ū
*		   ���̺��� �����Ѵ�.
*
* �Ű� : ����
* ��ȯ : ���� ���� = 0 , ���� = < 0
* ���� : ���� �ʱ� ���������� ������ ���� �˻縦 ���� �ʰ� �Ѿ �����̴�.
*	  ���� ������ ���� �˻� ��ƾ�� �߰��ؾ� �Ѵ�.
*
* -----------------------------------------------------------------------------------
*/
static int assem_pass1(void)
{
	/* add your code here */

	/* input_data�� ���ڿ��� ���پ� �Է� �޾Ƽ� 
	 * token_parsing()�� ȣ���Ͽ� token_unit�� ����
	 */

	for (int i = 0; i < line_num; i++)
	{
		token_parsing(input_data[i]);
	}
	return 0;
}

/* --------------------------------------
--------------------------------------------
* ���� : �Էµ� ���ڿ��� �̸��� ���� ���Ͽ� ���α׷��� ����� �����ϴ� �Լ��̴�.
*        ���⼭ ��µǴ� ������ ��ɾ� ���� OPCODE�� ��ϵ� ǥ(���� 4��) �̴�.
* �Ű� : ������ ������Ʈ ���ϸ�
* ��ȯ : ����
* ���� : ���� ���ڷ� NULL���� ���´ٸ� ���α׷��� ����� ǥ��������� ������
*        ȭ�鿡 ������ش�.
*        ���� ���� 4�������� ���̴� �Լ��̹Ƿ� ������ ������Ʈ������ ������ �ʴ´�.
* -----------------------------------------------------------------------------------
*/
void make_opcode_output(char *file_name)
{
	/* add your code here */
	FILE *file = fopen(file_name, "w");
	char buffer[128] = {
		0,
	};
	for (int i = 0; i < token_line; i++)
	{
		//label ���
		if (token_table[i]->label)
		{
			fputs(token_table[i]->label, file);
			fputs("\t", file);
		}
		else
			fputs("\t", file);
		//operator ���
		if (token_table[i]->operator)
		{
			fputs(token_table[i]->operator, file);
			fputs("\t", file);
		}
		else
			fputs("\t", file);

		//operand ���
		for (int k = 0; k < 3; k++)
		{
			if (token_table[i]->operand[k])
			{
				if (k == 0)
					fputs(token_table[i]->operand[k], file);
				else
				{
					fputs(",", file);
					fputs(token_table[i]->operand[k], file);
				}
			}
			else
			{
				fputs("\n", file);
				break;
			}
		}
	}
	return;
}

/* ----------------------------------------------------------------------------------
* ���� : �Էµ� ���ڿ��� �̸��� ���� ���Ͽ� ���α׷��� ����� �����ϴ� �Լ��̴�.
*        ���⼭ ��µǴ� ������ SYMBOL�� �ּҰ��� ����� TABLE�̴�.
* �Ű� : ������ ������Ʈ ���ϸ�
* ��ȯ : ����
* ���� : ���� ���ڷ� NULL���� ���´ٸ� ���α׷��� ����� ǥ��������� ������
*        ȭ�鿡 ������ش�.
*
* -----------------------------------------------------------------------------------
*/
void make_symtab_output(char *file_name)
{
	/* add your code here */
}

/* ----------------------------------------------------------------------------------
* ���� : �Էµ� ���ڿ��� �̸��� ���� ���Ͽ� ���α׷��� ����� �����ϴ� �Լ��̴�.
*        ���⼭ ��µǴ� ������ LITERAL�� �ּҰ��� ����� TABLE�̴�.
* �Ű� : ������ ������Ʈ ���ϸ�
* ��ȯ : ����
* ���� : ���� ���ڷ� NULL���� ���´ٸ� ���α׷��� ����� ǥ��������� ������
*        ȭ�鿡 ������ش�.
*
* -----------------------------------------------------------------------------------
*/
void make_literaltab_output(char *filen_ame)
{
	/* add your code here */
}

/* --------------------------------------------------------------------------------*
* ------------------------- ���� ������Ʈ���� ����� �Լ� --------------------------*
* --------------------------------------------------------------------------------*/

/* ----------------------------------------------------------------------------------
* ���� : ����� �ڵ带 ���� �ڵ�� �ٲٱ� ���� �н�2 ������ �����ϴ� �Լ��̴�.
*		   �н� 2������ ���α׷��� ����� �ٲٴ� �۾��� ���� ������ ����ȴ�.
*		   ������ ���� �۾��� ����Ǿ� ����.
*		   1. ������ �ش� ����� ��ɾ ����� �ٲٴ� �۾��� �����Ѵ�.
* �Ű� : ����
* ��ȯ : �������� = 0, �����߻� = < 0
* ���� :
* -----------------------------------------------------------------------------------
*/
static int assem_pass2(void)
{

	/* add your code here */
}

/* ----------------------------------------------------------------------------------
* ���� : �Էµ� ���ڿ��� �̸��� ���� ���Ͽ� ���α׷��� ����� �����ϴ� �Լ��̴�.
*        ���⼭ ��µǴ� ������ object code (������Ʈ 1��) �̴�.
* �Ű� : ������ ������Ʈ ���ϸ�
* ��ȯ : ����
* ���� : ���� ���ڷ� NULL���� ���´ٸ� ���α׷��� ����� ǥ��������� ������
*        ȭ�鿡 ������ش�.
*
* -----------------------------------------------------------------------------------
*/
void make_objectcode_output(char *file_name)
{
	/* add your code here */
}
