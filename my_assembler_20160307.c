/*
 * 화일명 : my_assembler_20160307.c 
 * 설  명 : 이 프로그램은 SIC/XE 머신을 위한 간단한 Assembler 프로그램의 메인루틴으로,
 * 입력된 파일의 코드 중, 명령어에 해당하는 OPCODE를 찾아 출력한다.
 * 파일 내에서 사용되는 문자열 "00000000"에는 자신의 학번을 기입한다.
 */

/*
 *
 * 프로그램의 헤더를 정의한다. 
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include "my_assembler_20160307.h"
#pragma warning(disable: 4996)

/* ----------------------------------------------------------------------------------
 * 설명 : 사용자로 부터 어셈블리 파일을 받아서 명령어의 OPCODE를 찾아 출력한다.
 * 매계 : 실행 파일, 어셈블리 파일 
 * 반환 : 성공 = 0, 실패 = < 0 
 * 주의 : 현재 어셈블리 프로그램의 리스트 파일을 생성하는 루틴은 만들지 않았다. 
 *		   또한 중간파일을 생성하지 않는다. 
 * ----------------------------------------------------------------------------------
 */
int main(int args, char *arg[])
{
	if (init_my_assembler() < 0)
	{
		printf("init_my_assembler: 프로그램 초기화에 실패 했습니다.\n");
		return -1;
	}

	if (assem_pass1() < 0)
	{
		printf("assem_pass1: 패스1 과정에서 실패하였습니다.  \n");
		return -1;
	}
	//make_opcode_output(NULL);
	 make_symtab_output("Symtab_output_20160307.txt");
	 make_literaltab_output("literaltab_output_20160307.txt");
	 if (assem_pass2() < 0)
	 {
	 	printf("assem_pass2: 패스2 과정에서 실패하였습니다.  \n");
	 	return -1;
	 }

	// make_objectcode_output("output_00000000");
	return 0;
}

/* ----------------------------------------------------------------------------------
 * 설명 : 프로그램 초기화를 위한 자료구조 생성 및 파일을 읽는 함수이다. 
 * 매계 : 없음
 * 반환 : 정상종료 = 0 , 에러 발생 = -1
 * 주의 : 각각의 명령어 테이블을 내부에 선언하지 않고 관리를 용이하게 하기 
 *		   위해서 파일 단위로 관리하여 프로그램 초기화를 통해 정보를 읽어 올 수 있도록
 *		   구현하였다. 
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
 * 설명 : 머신을 위한 기계 코드목록 파일을 읽어 기계어 목록 테이블(inst_table)을 
 *        생성하는 함수이다. 
 * 매계 : 기계어 목록 파일
 * 반환 : 정상종료 = 0 , 에러 < 0 
 * 주의 : 기계어 목록파일 형식은 자유롭게 구현한다. 예시는 다음과 같다.
 *	
 *	===============================================================================
 *		   | 이름 | 형식 | 기계어 코드 | 오퍼랜드의 갯수 | NULL|
 *	===============================================================================	   
 *		
 * ----------------------------------------------------------------------------------
 */

//inst 메모리 할당 함수 . 토큰 제어 포함
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
			//mnemonic 값 토큰 복사
			memory_allocation(&(inst_table[i]->mnemonic), &inst_token);

			//operand_count 값 토큰 복사
			inst_table[i]->operand_count = atoi(inst_token);
			inst_token = strtok(NULL, "\t");

			//format 값 토큰 복사
			memory_allocation(&(inst_table[i]->format), &inst_token);

			//opcode 값 토큰 복사
			inst_token[strlen(inst_token) - 1] = '\0'; //개행문자 제거
			memory_allocation(&(inst_table[i]->opcode), &inst_token);
			inst_index++;
		}
	}
	fclose(file);
	return errno;
}

/* ----------------------------------------------------------------------------------
 * 설명 : 어셈블리 할 소스코드를 읽어 소스코드 테이블(input_data)를 생성하는 함수이다. 
 * 매계 : 어셈블리할 소스파일명
 * 반환 : 정상종료 = 0 , 에러 < 0  
 * 주의 : 라인단위로 저장한다.
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
		//마지막 input파일에 있는 공백 무시
		if (buffer[0] == NULL)
		{
			continue;
		}
		input_data[i] = (char *)malloc(sizeof(buffer));
		//개행문자 제거함
		if(buffer[strlen(buffer) - 1] == '\n')
		buffer[strlen(buffer) - 1] = '\0';
		strcpy(input_data[i], buffer);
		line_num++;
	}
	fclose(file);
	return errno;
}

/* ----------------------------------------------------------------------------------
 * 설명 : 소스 코드를 읽어와 토큰단위로 분석하고 토큰 테이블을 작성하는 함수이다. 
 *        패스 1로 부터 호출된다. 
 * 매계 : 파싱을 원하는 문자열  
 * 반환 : 정상종료 = 0 , 에러 < 0 
 * 주의 : my_assembler 프로그램에서는 라인단위로 토큰 및 오브젝트 관리를 하고 있다. 
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
	token_table->nixbpe = 0;
}
int token_parsing(char *str)
{
	/* add your code here */
	char *parsing_token = NULL;
	char *sub_parsing_token = NULL; //여러개의 operand 가 있을경우 사용할 토큰
	int islabel = 0;				// 0 : label 없음, 1 : label 존재
	int LTORG_index = 0;			//리터럴 문자제어 index 나타낼 변수
	// int before_literal_count = 0;			//할당을 마친 이전의 literal 개수

	char str_buffer[100] = {
		0,
	};

	//토큰 초기화
	token_table[token_line] = (token *)malloc(sizeof(token));
	clear_token_table(token_table[token_line]);
	strcpy(str_buffer, str);

	//라벨의 유무 검사
	if (str_buffer[0] != '\t')
		islabel = 1;
	parsing_token = strtok(str_buffer, "\t");

	//"." , "\n"으로 시작하는 주석 문장. 필요 없으므로 바로 메모리 해제
	if ((str_buffer[0] == '.') || (str_buffer[0] == '\n'))
	{
		free(token_table[token_line]);
		return 0;
	}
	while (parsing_token != NULL)
	{
		/********************Label 인자 할당 부분************************/
		if (islabel == 1)
		{
			memory_allocation(&(token_table[token_line]->label), &parsing_token);
		}
		//특별 : RSUB 는 따로 예외 처리
		if (!(strcmp(parsing_token, "RSUB")))
		{
			memory_allocation(&(token_table[token_line]->operator), &parsing_token);
			memory_allocation(&(token_table[token_line]->comment), &parsing_token);
			token_line++;
			break;
		}
		/*******************Literal 할당 부분 *******************/
		//만약 LTORG일 경우
		if (!(strcmp(parsing_token, "LTORG")))
		{
			LTORG_index = token_line;
			if (literal_count > 0)
			{
				for (int i = 0; i < literal_count; i++)
				{
					//토큰테이블 생성및 초기화
					token_table[++token_line] = (token *)malloc(sizeof(token));
					clear_token_table(token_table[token_line]);
					token_table[token_line]->operator = (char*)malloc(sizeof(char) * 10);
					//리터럴 테이블에서 토큰 테이블로 할당
					literal_table[literal_index].token_index = token_line;
					strcpy(token_table[token_line]->operator, literal_table[literal_index++].literal);
				}
				literal_count = 0;
			}
			//할당받지 못한 literal이 없을 경우
			memory_allocation(&(token_table[token_line - 1]->operator), &parsing_token);
			token_line++;
			break;
		}
		//파일 종료시까지 할당되지 않은 Literal 문자 존재할 경우
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
			//end 의 operand
			memory_allocation(&(token_table[token_line - 1]->operand[0]), &parsing_token);
			token_line++;
			break;

		}
			/********************operator 인자 할당 부분************************/
		else {
			memory_allocation(&(token_table[token_line]->operator), &parsing_token);
			//CSECT 처리
			if (parsing_token == NULL)
			{
				token_line++;
				break;
			}
		}
		/********************operand 인자 할당 부분************************/
		//Operand 인자가 여러개 인 경우 ","로 구분되는 sub 토큰
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
			//comment 인자 없을 경우 종료
			if (!(parsing_token = strtok(NULL, "\t")))
			{
				token_line++;
				break;
			}
		}
		//operand인자가 1개 인 경우
		else
		{
			/***************Literal 문자를 만날 경우, literal_table 생성하는 부분*********************/
			if (parsing_token[0] == '=')
			{
				//TODO : 중복된 리터럴 있을 시 0 , 없을 시 1.
				if (search_literal_table(parsing_token)){
				literal_count++;
				// token_table 의 index와 문자 넣어줌
				//literal_table[literal_line].token_index = token_line;
				strcpy(literal_table[literal_line++].literal, parsing_token);
				}
			}
			memory_allocation(&(token_table[token_line]->operand[0]), &parsing_token);
			//comment 인자 없을 경우 종료
			if (parsing_token == NULL)
			{
				token_line++;
				break;
			}
		}

		//comment 부분 동적 할당
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
 * 설명 : 입력 문자열이 기계어 코드인지를 검사하는 함수이다. 
 * 매계 : 토큰 단위로 구분된 문자열 
 * 반환 : 정상종료 = 기계어 테이블 인덱스, 에러 < 0 
 * 주의 : 
 *		
 * ----------------------------------------------------------------------------------
 */
//주어진  operator의 값과 같은 inst_table 인덱스 리턴
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
* 설명 : 어셈블리 코드를 위한 패스1과정을 수행하는 함수이다.
*		   패스1에서는..
*		   1. 프로그램 소스를 스캔하여 해당하는 토큰단위로 분리하여 프로그램 라인별 토큰
*		   테이블을 생성한다.
*
* 매계 : 없음
* 반환 : 정상 종료 = 0 , 에러 = < 0
* 주의 : 현재 초기 버전에서는 에러에 대한 검사를 하지 않고 넘어간 상태이다.
*	  따라서 에러에 대한 검사 루틴을 추가해야 한다.
*
* -----------------------------------------------------------------------------------
*/
static int assem_pass1(void)
{
	/* add your code here */

	/* input_data의 문자열을 한줄씩 입력 받아서 
	 * token_parsing()을 호출하여 token_unit에 저장
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

	// 각 토큰테이블에 location 할당 
	for (int index = 0; index < token_line; index++) {
		if (index == (token_line - 1)) {
			//프로그램 마지막줄 실행시, 길이 session_length배열에 넣어줌
			session[session_count].session_length = location_allocation(token_table[index]->operator,index);
			break;
		}
		//token_table[index+1]->location = location_allocation(token_table[index]->operator,index);
		address[address_line].operator = token_table[index]->operator;	//address 구조체에 메모리 주소값 넣어줌
		address[++address_line].location = location_allocation(token_table[index]->operator,index);	//다음 operator시작 주소에 값을 더해줌
	}


	return errno;
}
/***********************************************
 * 메모리 주소 할당 함수. Locctr 값 리턴.
 * *********************************************/
int location_allocation(char *str, int token_line)
{
	int index = search_opcode(str);
	// opcode 없을 시, -1 return
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
		if ((!(strcmp(str, "START")))) {
			//시작한 프로그램의 이름 session  배열에 넣어줌
			strcpy(session[session_count].session_name, token_table[token_line]->label);
		}
		if ((!(strcmp(str, "CSECT"))))
		{
			//CSECT 만날 시, 각 세션의 길이를 session 배열에 넣어줌
			//session[session_count++].session_length = token_table[token_line]->location;
			session[session_count++].session_length = address[token_line].location;
			strcpy(session[session_count].session_name, token_table[token_line]->label);
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
		//리터럴 상수 
		if (str[0] == '=') {
			//작은 따옴표의 갯수 빼줌
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
			//16진수로 시작시, operand 크기  계산해서 더함
			if (strchr(temp, 'X')) {
				while (*temp != '\'')
					temp++;
				locctr += (int)((strlen(temp) - 2) * 0.5);
				return locctr;
			}
		}
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
* 설명 : 입력된 문자열의 이름을 가진 파일에 프로그램의 결과를 저장하는 함수이다.
*        여기서 출력되는 내용은 명령어 옆에 OPCODE가 기록된 표(과제 4번) 이다.
* 매계 : 생성할 오브젝트 파일명
* 반환 : 없음
* 주의 : 만약 인자로 NULL값이 들어온다면 프로그램의 결과를 표준출력으로 보내어
*        화면에 출력해준다.
*        또한 과제 4번에서만 쓰이는 함수이므로 이후의 프로젝트에서는 사용되지 않는다.
* -----------------------------------------------------------------------------------
*/
void make_opcode_output(char *file_name)
{
	char buffer[128] = {
		0,
	};
	int operator_index = 0;
	// 파일 이름이 NULL인 경우 표준툴력을 해준다.
	if (file_name == NULL)
	{
		// 파일 이름이 NULL 이 아닌 경우
		for (int i = 0; i < token_line; i++)
		{
			//label 출력
			if (token_table[i]->label)
			{
				fputs(token_table[i]->label, stdout);
				fputs("\t", stdout);
			}
			else
				fputs("\t", stdout);
			//operator 출력
			if (token_table[i]->operator)
			{
				fputs(token_table[i]->operator, stdout);
				fputs("\t", stdout);
			}
			else
				fputs("\t", stdout);

			//operand 출력
			for (int k = 0; k < 3; k++)
			{
				//operand 갯수는 가변적이기 때문에 operand값이 null 이 아닐때 까지 출력
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
			//opcode를 search_opcode 메소드를 활용하여 inst_table에서 찾아서 출력.
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
* 설명 : 입력된 문자열의 이름을 가진 파일에 프로그램의 결과를 저장하는 함수이다.
*        여기서 출력되는 내용은 SYMBOL별 주소값이 저장된 TABLE이다.
* 매계 : 생성할 오브젝트 파일명
* 반환 : 없음
* 주의 : 만약 인자로 NULL값이 들어온다면 프로그램의 결과를 표준출력으로 보내어
*        화면에 출력해준다.
*
* -----------------------------------------------------------------------------------
*/
//심볼의 주소값 리턴, 없을 시, -1 리턴
int find_addr_symbol_from_table(char *str) {
	for (int i = 0; i < symbol_line;i++) {
		if (!(strcmp(sym_table[i].symbol, str))) {
			return sym_table[i].addr;
		}
	}
	return -1;
}
void make_symtab_output(char *file_name)
{
	//3개의 session을 구분해줄 값
	int session_number = 0;
	int buffer_addr = 0, bufend_addr = 0;
	//Token_table 에서 label값 symbol로 만들어줌
	for (int i = 0; i < token_line;i++) {
		//Symbol 값 존재할 떄,
		if (token_table[i]->label) {
			strcpy(sym_table[symbol_line].symbol, token_table[i]->label);
			sym_table[symbol_line].addr=token_table[i]->location;
			//RDREC 섹션 구분
			if (!(strcmp(token_table[i]->label, "RDREC"))) {
				session_number++;
				sym_table[symbol_line].addr = 0;
			}
			//WRREC 섹션 구분
			if (!(strcmp(token_table[i]->label, "WRREC"))) {
				session_number++;
				sym_table[symbol_line].addr = 0;
			}
			sym_table[symbol_line].session = session_number;
			//첫번째 session의 MAXLEN 값 할당
			if (!(strcmp(token_table[i]->label, "MAXLEN")) && (sym_table[symbol_line].session == 0)) {
				buffer_addr = find_addr_symbol_from_table("BUFFER");
				bufend_addr = find_addr_symbol_from_table("BUFEND");
				sym_table[symbol_line].addr = bufend_addr - buffer_addr;
			}
			symbol_line++;

		}
	}

	
	//인자가 NULL 일 경우, 표준 출력
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
	// 입력받은 file_name 인자에 출력
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
* 설명 : 입력된 문자열의 이름을 가진 파일에 프로그램의 결과를 저장하는 함수이다.
*        여기서 출력되는 내용은 LITERAL별 주소값이 저장된 TABLE이다.
* 매계 : 생성할 오브젝트 파일명
* 반환 : 없음
* 주의 : 만약 인자로 NULL값이 들어온다면 프로그램의 결과를 표준출력으로 보내어
*        화면에 출력해준다.
*
* -----------------------------------------------------------------------------------
*/
//literal 문자의 특수문자 지워주는 함수 
char* literal_erase_quote(char* ptr) {
	char* buffer = (char*)malloc(sizeof(char) * 10);
	while (*ptr != '\'')
	{
		ptr++;
	}
	strcpy(buffer, ptr + 1);
	buffer[strlen(buffer) - 1] = '\0';
	return buffer;
}

int find_addr_literal_from_table(char *str) {
	for (int i = 0; i < literal_line;i++) {
		if (!(strcmp(literal_table[i].literal, str))) {
			return literal_table[i].addr;
		}
	}
	return -1;
}
//literal 테이블 출력
void make_literaltab_output(char* file_name)
{
	char* temp = NULL;
	//literal 주소 할당
	for (int i = 0; i < literal_line; i++) {
		//literal의 주소를 가지고 있는 token_table
		int token_index = literal_table[i].token_index;
		literal_table[i].addr = token_table[token_index]->location;
	}
	//인자의 값이 NULL일 때,
	if (file_name == NULL)
	{
		for (int i = 0; i < literal_line; i++)
		{
			if (literal_table[i].literal)
			{
				//'=' , ''', 과 같은 특수 문자 제거 후 출력
				temp = literal_erase_quote(&(literal_table[i].literal));
				fputs(temp, stdout);
				fprintf(stdout, "\t%X\n", literal_table[i].addr);
			}
		}
	}
	// 입력받은 인자에 출력
	else
	{
		FILE* file = fopen(file_name, "w");
		for (int i = 0; i < literal_line; i++)
		{
			if (literal_table[i].literal)
			{
				//'=' , ''', 과 같은 특수 문자 제거 후 출력
				temp = literal_erase_quote(&(literal_table[i].literal));
				fputs(temp, file);
				fprintf(file, "\t%X\n", literal_table[i].addr);
			}
		}
	}
}

/* ----------------------------------------------------------------------------------
* 설명 : 어셈블리 코드를 기계어 코드로 바꾸기 위한 패스2 과정을 수행하는 함수이다.
*		   패스 2에서는 프로그램을 기계어로 바꾸는 작업은 라인 단위로 수행된다.
*		   다음과 같은 작업이 수행되어 진다.
*		   1. 실제로 해당 어셈블리 명령어를 기계어로 바꾸는 작업을 수행한다.
* 매계 : 없음
* 반환 : 정상종료 = 0, 에러발생 = < 0
* 주의 :
* -----------------------------------------------------------------------------------
*/
static int assem_pass2(void)
{
	char bits = 0;
	char* pinst_opcode = NULL;
	char address_buffer[10] = { 0, };	//상대주소 연산을 위한 버퍼
	char object_code[10] = { 0, };		//object_code를 담을 버퍼
	int inst_index = 0;		//명령어 opcode 할당을 위한 index
	//토큰 테이블에 object코드 값 할당.
	for (int i = 0; i < token_line; i++) {
		//TODO : 리터럴 값과 심볼값 들어가있는 방식 확인하기
		//inst 가 존재시 ,opcode 가져옴
		inst_index = search_opcode(token_table[i]->operator);
		if (inst_index > 0) {
			pinst_opcode = inst_table[inst_index]->opcode;
			//object_code의 배열에 명령어 opcode 첫번째 값 넣어줌
			address_buffer[0] =  pinst_opcode[0];
			//opcode 의 값에 따른 주소 값
			if (!(strcmp(inst_table[inst_index]->format, "3/4"))) {
				switch (*(pinst_opcode + 1))
				{
				case 'C':
					token_table[i]->nixbpe = 0x03;
					break;
				case '4':
					token_table[i]->nixbpe = 0x01;
					break;
				case '8':
					token_table[i]->nixbpe = 0x02;
					break;
				case '0':
					token_table[i]->nixbpe = 0x00;
					break;

				}
					token_table[i]->nixbpe <<= 6;	//왼쪽 끝으로 보낸다.
				/*********operand 에 따른 주조시정 방식 값. 형식 확인 후, 특수문자 제거**************/
				char** ptr = NULL;
				if (token_table[i]->operand[0]) {
					ptr = &(token_table[i]->operand);
					//리터럴
				/*	if (ptr[0][0] == '=') {

					}*/
					if (ptr[0][0] == '#') { //imediate.
						token_table[i]->nixbpe |= 0x10;
						token_table[i]->operand[0] = token_table[i]->operand[0] + 1;
					}
					else if (ptr[0][0] == '@') { //indirect
						token_table[i]->nixbpe |= 0x20;
						token_table[i]->operand[0] = token_table[i]->operand[0] + 1;
					}
					//루프를 도는 문장일 때,
					else if ((ptr[1] != NULL) && (ptr[1][0]=='X')) { //roop
						token_table[i]->nixbpe |= 0x38;
					}
					else {	//simple addressing
						token_table[i]->nixbpe |= 0x30;
					}
				}
				//operand가 존재하지 않는 RSUB
				else {
					token_table[i]->nixbpe |= 0x30;
					printf("%s의 object 값 : %s\n", token_table[i]->operator,object_code);
					continue;
				}
				/******PC-relative 및 Direct-addressing 주소 계산*******/
				//4형식, 직접 주소 지정방식
				if (token_table[i]->operator[0] == '+') {
					token_table[i]->nixbpe |= 0x01;
				}
				/*
				//리터럴 방식
				else if () {

				}
				//imediate 방식
				else if () {

				}
				*/
				else {
				//operand의 값이 심볼 테이블에 있는지 검사. 있으면 pc-relative
					int address_symbol = 0;
					address_symbol = find_addr_symbol_from_table(token_table[i]->operand[0]);
					if (address_symbol >= 0) {
						token_table[i]->nixbpe |= 0x02;
					}
				}
				//nixpbe 값 object 배열에 넣어줌
				//명령어 배열 만들기
				object_code[0] = token_table[i]->nixbpe >> 4;	//연산의 편리성을 위해 4비트 밀어줌
				object_code[0] &= 0x0F;							//음수 비트 제거
				object_code[1] = token_table[i]->nixbpe & 0x0F; //음수 비트 제거
				sprintf(&(address_buffer[1]), "%X", object_code[0]);	//adress 버퍼에 16진수형으로 복사
				sprintf(&(address_buffer[2]), "%X", object_code[1]);	//adress 버퍼에 16진수형으로 복사
				printf("%s의 object 값 : %s\n", token_table[i]->operator,address_buffer);
				//printf("%s의 object 값 : %s\n", token_table[i]->operator,address_buffer);


				//printf("%s의 주소 : %d, opcode 값 : %s\tnixbpe 값 : %d\n",token_table[i]->operator,token_table[i]->location, inst_table[inst_index]->opcode,token_table[i]->nixbpe);

			}
			//TODO : format이 2형식인 경우
			else {
				//token_table[i]->operator
				printf("%s의 object 값 : %s\n", token_table[i]->operator,object_code);
			}
		}
		//operator의 값이 없는 부분.
		else {
			//리터럴타입 이나 resb 관리..
			continue;
		}



	}
	return 0;
}

/* ----------------------------------------------------------------------------------
* 설명 : 입력된 문자열의 이름을 가진 파일에 프로그램의 결과를 저장하는 함수이다.
*        여기서 출력되는 내용은 object code (프로젝트 1번) 이다.
* 매계 : 생성할 오브젝트 파일명
* 반환 : 없음
* 주의 : 만약 인자로 NULL값이 들어온다면 프로그램의 결과를 표준출력으로 보내어
*        화면에 출력해준다.
*
* -----------------------------------------------------------------------------------
*/
void make_objectcode_output(char *file_name)
{
	/* add your code here */
}
