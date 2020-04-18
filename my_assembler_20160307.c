/*
 * ȭ�ϸ� : my_assembler_20160307.c 
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
#include "my_assembler_20160307.h"
#pragma warning(disable: 4996)

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
	if (init_my_assembler() < 0)
	{
		printf("init_my_assembler: ���α׷� �ʱ�ȭ�� ���� �߽��ϴ�.\n");
		return -1;
	}

	if (assem_pass1() < 0)
	{
		printf("assem_pass1: �н�1 �������� �����Ͽ����ϴ�.  \n");
		return -1;
	}
	//make_opcode_output(NULL);
	 make_symtab_output("Symtab_output_20160307.txt");
	 make_literaltab_output("literaltab_output_20160307.txt");
	// if (assem_pass2() < 0)
	// {
	// 	printf("assem_pass2: �н�2 �������� �����Ͽ����ϴ�.  \n");
	// 	return -1;
	// }

	// make_objectcode_output("output_00000000");
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
		//������ input���Ͽ� �ִ� ���� ����
		if (buffer[0] == NULL)
		{
			continue;
		}
		input_data[i] = (char *)malloc(sizeof(buffer));
		//���๮�� ������
		if(buffer[strlen(buffer) - 1] == '\n')
		buffer[strlen(buffer) - 1] = '\0';
		strcpy(input_data[i], buffer);
		line_num++;
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
	token_table->location = 0;
}
int token_parsing(char *str)
{
	/* add your code here */
	char *parsing_token = NULL;
	char *sub_parsing_token = NULL; //�������� operand �� ������� ����� ��ū
	int islabel = 0;				// 0 : label ����, 1 : label ����
	int LTORG_index = 0;			//���ͷ� �������� index ��Ÿ�� ����
	// int before_literal_count = 0;			//�Ҵ��� ��ģ ������ literal ����

	char str_buffer[100] = {
		0,
	};

	//��ū �ʱ�ȭ
	token_table[token_line] = (token *)malloc(sizeof(token));
	clear_token_table(token_table[token_line]);
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
		/********************Label ���� �Ҵ� �κ�************************/
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
			break;
		}
		/*******************Literal �Ҵ� �κ� *******************/
		//���� LTORG�� ���
		if (!(strcmp(parsing_token, "LTORG")))
		{
			LTORG_index = token_line;
			if (literal_count > 0)
			{
				for (int i = 0; i < literal_count; i++)
				{
					//��ū���̺� ������ �ʱ�ȭ
					token_table[++token_line] = (token *)malloc(sizeof(token));
					clear_token_table(token_table[token_line]);
					token_table[token_line]->operator = (char*)malloc(sizeof(char) * 10);
					//���ͷ� ���̺��� ��ū ���̺�� �Ҵ�
					literal_table[literal_index].token_index = token_line;
					strcpy(token_table[token_line]->operator, literal_table[literal_index++].literal);
				}
				literal_count = 0;
			}
			//�Ҵ���� ���� literal�� ���� ���
			memory_allocation(&(token_table[token_line - 1]->operator), &parsing_token);
			token_line++;
			break;
		}
		//���� ����ñ��� �Ҵ���� ���� Literal ���� ������ ���
		if (!(strcmp(parsing_token, "END")) && (literal_count > 0)) {

			for (int i = 0; i < literal_count; i++)
			{
				token_table[++token_line] = (token*)malloc(sizeof(token));
				clear_token_table(token_table[token_line]);
				token_table[token_line]->operator = (char*)malloc(sizeof(char) * 10);

				literal_table[literal_index].token_index = token_line;
				strcpy(token_table[token_line]->operator, literal_table[literal_index++].literal);
			}
			literal_count = 0;
			memory_allocation(&(token_table[token_line - 1]->operator), &parsing_token);
			//end �� operand
			memory_allocation(&(token_table[token_line - 1]->operand[0]), &parsing_token);
			token_line++;
			break;

		}
			/********************operator ���� �Ҵ� �κ�************************/
		else {
			memory_allocation(&(token_table[token_line]->operator), &parsing_token);
			//CSECT ó��
			if (parsing_token == NULL)
			{
				token_line++;
				break;
			}
		}
		/********************operand ���� �Ҵ� �κ�************************/
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
				break;
			}
		}
		//operand���ڰ� 1�� �� ���
		else
		{
			/***************Literal ���ڸ� ���� ���, literal_table �����ϴ� �κ�*********************/
			if (parsing_token[0] == '=')
			{
				//TODO : �ߺ��� ���ͷ� ���� �� 0 , ���� �� 1.
				if (search_literal_table(parsing_token)){
				literal_count++;
				// token_table �� index�� ���� �־���
				//literal_table[literal_line].token_index = token_line;
				strcpy(literal_table[literal_line++].literal, parsing_token);
				}
			}
			memory_allocation(&(token_table[token_line]->operand[0]), &parsing_token);
			//comment ���� ���� ��� ����
			if (parsing_token == NULL)
			{
				token_line++;
				break;
			}
		}

		//comment �κ� ���� �Ҵ�
		memory_allocation(&(token_table[token_line]->comment), &parsing_token);
		token_line++;
	}

	return 0;
}
int search_literal_table(char *str) {
	for (int i = 0; i < literal_line; i++) {
		if (!(strcmp(str, literal_table[i].literal)))
			return 0;
	}
		return 1;
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
	char *temp = str;
	if (str == NULL)
		return -1;
	if (str[0] == '+')
		temp = str + 1;
	/* add your code here */
	for (int i = 0; i < inst_index; i++)
	{
		if (!(strcmp(inst_table[i]->mnemonic, temp)))
		{
			return i;
		}
	}
	return -1;
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
	int errno;
	for (int i = 0; i < MAX_INST; i++)
	{
		if (input_data[i] ==NULL){
			break;
		}
		else
			if (token_parsing(input_data[i]) < 0) {
				errno = -1;
				return errno;

		}
	}

	//TODO : location �Ҵ� �κ�
	for (int index = 0; index < token_line; index++) {
		if (index == (token_line - 1)) {
			session_length[session_count] = location_allocation(token_table[index]->operator,index);
			break;
		}
		token_table[index+1]->location = location_allocation(token_table[index]->operator,index);
	}


	return errno;
}
/***********************************************
 * �޸� �ּ� �Ҵ� �Լ�. Locctr �� ����.
 * *********************************************/
int location_allocation(char *str, int token_line)
{
	int index = search_opcode(str);
	// opcode ���� ��, -1 return
	if (index == -1)
	{
		if ((!(strcmp(str, "EXTDEF"))) || (!(strcmp(str, "EXTREF"))))
		{
			return 0;
		}
		if ((!(strcmp(str, "EQU"))) || (!(strcmp(str, "LTORG"))))
		{
			return locctr;
		}
		if ((!(strcmp(str, "CSECT"))) || (!(strcmp(str, "START"))))
		{
			//�� ������ ���̸� session_length �迭�� �־���
			session_length[session_count++] = token_table[token_line]->location;
			locctr = 0;
			return locctr;
		}
		if (!(strcmp(str, "END"))) {
			return locctr;
		}
		if (!(strcmp(str, "RESW")))
		{
			locctr += atoi(token_table[token_line]->operand[0]) * 3;
			return locctr;
		}
		if (!(strcmp(str, "RESB")))
		{
			locctr += atoi(token_table[token_line]->operand[0]);
			return locctr;
		}
		if (!(strcmp(str, "WORD"))) {
			locctr += 3;
			return locctr;
		}
		//���ͷ� ��� 
		if (str[0] == '=') {
			//���� ����ǥ�� ���� ����
			if (strchr(str, 'C')) {
			while (*str != '\'')
				str++;
				locctr += (int)((strlen(str) - 2) * 1);
				return locctr;
			}
			else if (strchr(str, 'X')) {
				while (*str != '\'')
					str++;
				locctr += (int)((strlen(str)-2) * 0.5);
				return locctr;
			}
		}
		if (!(strcmp(str, "BYTE"))) {
			char* temp = NULL;
			temp = token_table[token_line]->operand[0];
			//16������ ���۽�, operand ũ��  ����ؼ� ����
			if (strchr(temp, 'X')) {
				while (*temp != '\'')
					temp++;
				locctr += (int)((strlen(temp) - 2) * 0.5);
				return locctr;
			}
		}
		//TODO MAXLEN
	}
	else
	{
		if (str[0] == '+')
		{
			locctr += 4;
			return locctr;
		}
		if (!(strcmp(inst_table[index]->format, "2")))
		{
			locctr += 2;
			return locctr;
		}
		if (!(strcmp(inst_table[index]->format, "3/4")))
		{
			locctr += 3;
			return locctr;
		}
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
	char buffer[128] = {
		0,
	};
	int operator_index = 0;
	// ���� �̸��� NULL�� ��� ǥ�������� ���ش�.
	if (file_name == NULL)
	{
		// ���� �̸��� NULL �� �ƴ� ���
		for (int i = 0; i < token_line; i++)
		{
			//label ���
			if (token_table[i]->label)
			{
				fputs(token_table[i]->label, stdout);
				fputs("\t", stdout);
			}
			else
				fputs("\t", stdout);
			//operator ���
			if (token_table[i]->operator)
			{
				fputs(token_table[i]->operator, stdout);
				fputs("\t", stdout);
			}
			else
				fputs("\t", stdout);

			//operand ���
			for (int k = 0; k < 3; k++)
			{
				//operand ������ �������̱� ������ operand���� null �� �ƴҶ� ���� ���
				if (token_table[i]->operand[k])
				{
					if (k == 0)
						fputs(token_table[i]->operand[k], stdout);
					else
					{
						fputs(",", stdout);
						fputs(token_table[i]->operand[k], stdout);
					}
				}
				else
				{
					fputs("\t", stdout);
					break;
				}
			}
			//opcode�� search_opcode �޼ҵ带 Ȱ���Ͽ� inst_table���� ã�Ƽ� ���.
			operator_index = search_opcode(token_table[i]->operator);
			if (operator_index != -1)
			{
				fputs(inst_table[operator_index]->opcode, stdout);
				fputs("\n", stdout);
			}
			else
			{
				fputs("\n", stdout);
			}
		}
	}
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
//�ɺ��� �ּҰ� ����
int find_addr_symbol_from_table(char *str) {
	for (int i = 0; i < symbol_line;i++) {
		if (!(strcmp(sym_table[i].symbol, str))) {
			return sym_table[i].addr;
		}
	}
}
void make_symtab_output(char *file_name)
{
	//3���� session�� �������� ��
	int session_number = 0;
	int buffer_addr = 0, bufend_addr = 0;
	//Token_table ���� label�� symbol�� �������
	for (int i = 0; i < token_line;i++) {
		//Symbol �� ������ ��,
		if (token_table[i]->label) {
			strcpy(sym_table[symbol_line].symbol, token_table[i]->label);
			sym_table[symbol_line].addr=token_table[i]->location;
			//RDREC ���� ����
			if (!(strcmp(token_table[i]->label, "RDREC"))) {
				session_number++;
				sym_table[symbol_line].addr = 0;
			}
			//WRREC ���� ����
			if (!(strcmp(token_table[i]->label, "WRREC"))) {
				session_number++;
				sym_table[symbol_line].addr = 0;
			}
			sym_table[symbol_line].session = session_number;
			//ù��° session�� MAXLEN �� �Ҵ�
			if (!(strcmp(token_table[i]->label, "MAXLEN")) && (sym_table[symbol_line].session == 0)) {
				buffer_addr = find_addr_symbol_from_table("BUFFER");
				bufend_addr = find_addr_symbol_from_table("BUFEND");
				sym_table[symbol_line].addr = bufend_addr - buffer_addr;
			}
			symbol_line++;

		}
	}

	
	//���ڰ� NULL �� ���, ǥ�� ���
	if (file_name == NULL)
	{
		for (int i = 0; i < symbol_line; i++)
		{
			if (sym_table[i].symbol)
			{
				if (!(strcmp(sym_table[i].symbol, "RDREC")) || !(strcmp(sym_table[i].symbol, "WRREC")))
					fputs("\n", stdout);
				fputs(sym_table[i].symbol, stdout);
				fprintf(stdout,"\t%X\n",sym_table[i].addr);
			}
		}
	}
	// �Է¹��� file_name ���ڿ� ���
	else
	{
		FILE *file = fopen(file_name, "w");
			for (int i = 0; i < symbol_line; i++)
			{
				if (sym_table[i].symbol)
				{
					if (!(strcmp(sym_table[i].symbol, "RDREC")) || !(strcmp(sym_table[i].symbol, "WRREC")))
						fputs("\n", file);
					fputs(sym_table[i].symbol, file);
					fprintf(file, "\t%X\n", sym_table[i].addr);
				}
			}
	}
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
//literal ������ Ư������ �����ִ� �Լ� 
char* literal_erase_quote(char * ptr) {
	char* buffer = (char*)malloc(sizeof(char) * 10);
	while (*ptr != '\'')
	{
		ptr++;
	}
	strcpy(buffer, ptr + 1);
	buffer[strlen(buffer) - 1] = '\0';
	return buffer;
}
//literal ���̺� ���
void make_literaltab_output(char* file_name)
{
	char* temp = NULL;
	//literal �ּ� �Ҵ�
	for (int i = 0; i < literal_line; i++) {
		//literal�� �ּҸ� ������ �ִ� token_table
		int token_index = literal_table[i].token_index;
		literal_table[i].addr = token_table[token_index]->location;
	}
	//������ ���� NULL�� ��,
	if (file_name == NULL)
	{
		for (int i = 0; i < literal_line; i++)
		{
			if (literal_table[i].literal)
			{
				//'=' , ''', �� ���� Ư�� ���� ���� �� ���
				temp = literal_erase_quote(&(literal_table[i].literal));
				fputs(temp, stdout);
				fprintf(stdout, "\t%X\n", literal_table[i].addr);
			}
		}
	}
	// �Է¹��� ���ڿ� ���
	else
	{
		FILE* file = fopen(file_name, "w");
		for (int i = 0; i < literal_line; i++)
		{
			if (literal_table[i].literal)
			{
				//'=' , ''', �� ���� Ư�� ���� ���� �� ���
				temp = literal_erase_quote(&(literal_table[i].literal));
				fputs(temp, file);
				fprintf(file, "\t%X\n", literal_table[i].addr);
			}
		}
	}
}

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
