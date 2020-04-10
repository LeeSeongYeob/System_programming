/*
 * 화일명 : my_assembler_00000000.c 
 * 설  명 : 이 프로그램은 SIC/XE 머신을 위한 간단한 Assembler 프로그램의 메인루틴으로,
 * 입력된 파일의 코드 중, 명령어에 해당하는 OPCODE를 찾아 출력한다.
 * 파일 내에서 사용되는 문자열 "00000000"에는 자신의 학번을 기입한다.
 */

/*
 *
 * 프로그램의 헤더를 정의한다. 
 *
 */
#define string(a) #a
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#define DEBUG 1

// 파일명의 "00000000"은 자신의 학번으로 변경할 것.
#include "my_assembler_00000000.h"

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
	// init_inst_file("inst.data");
	init_input_file("input.txt"); // Debug
	assem_pass1();

	// if (init_my_assembler() < 0)
	// {
	// 	printf("init_my_assembler: 프로그램 초기화에 실패 했습니다.\n");
	// 	return -1;
	// }

	// if (assem_pass1() < 0)
	// {
	// 	printf("assem_pass1: 패스1 과정에서 실패하였습니다.  \n");
	// 	return -1;
	// }
	// make_opcode_output("output_00000000");

	/*
	* 추후 프로젝트에서 사용되는 부분
	
	make_symtab_output("symtab_00000000");
	if(assem_pass2() < 0 ){
		printf(" assem_pass2: 패스2 과정에서 실패하였습니다.  \n") ; 
		return -1 ; 
	}

	make_objectcode_output("output_00000000") ; 
	*/
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

//
// char *inst_memory_allocation(char *parameter, char *inst_token)
// {
// 	parameter = (char *)malloc(strlen(inst_token));
// 	strcpy(parameter, inst_token);
// 	inst_token = strtok(NULL, "\t");
// 	return (parameter);
// }

void inst_memory_allocation(char **parameter, char **inst_token)
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
			inst_memory_allocation(&(inst_table[i]->mnemonic), &inst_token);

			//operand_count 값 토큰 복사
			inst_table[i]->operand_count = atoi(inst_token);
			inst_token = strtok(NULL, "\t");

			//format 값 토큰 복사
			inst_memory_allocation(&(inst_table[i]->format), &inst_token);

			//opcode 값 토큰 복사
			inst_token[strlen(inst_token) - 1] = '\0'; //개행문자 제거
			inst_memory_allocation(&(inst_table[i]->opcode), &inst_token);

			// inst_memory_allocation(inst_table[i]->mnemonic, inst_token);
			// inst_table[i]->mnemonic = (char *)malloc(strlen(inst_token));
			// strcpy(inst_table[i]->mnemonic, inst_token);
			// inst_token = strtok(NULL, "\t");
			// inst_table[i]->format = inst_memory_allocation(inst_table[i]->format, inst_token);

			//format 값 토큰 복사
			// inst_table[i]->format = (char *)malloc(strlen(inst_token));
			// strcpy(inst_table[i]->format, inst_token);
			// inst_token = strtok(NULL, "\t");

			//opcode 값 토큰 복사

			// inst_table[i]->opcode = (char *)malloc(strlen(inst_token));
			// strcpy(inst_table[i]->opcode, inst_token);
			// inst_token = strtok(NULL, "\t");

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
		input_data[i] = (char *)malloc(sizeof(buffer));
		//개행문자 제거함
		buffer[strlen(buffer) - 1] = '\0';
		strcpy(input_data[i], buffer);
		line_num++;

#ifdef DEBUG
		// printf("%s\n", input_data[i]);
#endif
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
}
int token_parsing(char *str)
{
	/* add your code here */
	char *parsing_token = NULL;
	char *sub_parsing_token = NULL; //여러개의 operand 가 있을경우 사용할 토큰
	int islabel = 0;				// 0 : label 없음, 1 : label 존재
	char str_buffer[256] = {
		0,
	};

	token_table[token_line] = (token *)malloc(sizeof(token));
	strcpy(str_buffer, str);
	//토큰 초기화
	clear_token_table(token_table[token_line]);

	//라벨의 유무 검사
	if (str[0] != '\t')
		islabel = 1;
	parsing_token = strtok(str_buffer, "\t");
	while (parsing_token != NULL)
	{
		//Label 이 존재하는 input line
		if (islabel == 1)
		{
			token_table[token_line]->label = (char *)malloc(strlen(parsing_token));
			strcpy(token_table[token_line]->label, parsing_token);
			parsing_token = strtok(NULL, "\t");
		}
		else
		{
			token_table[token_line]->label = NULL;
		}

		token_table[token_line]->operator=(char *) malloc(strlen(parsing_token));
		strcpy(token_table[token_line]->operator, parsing_token);
		parsing_token = strtok(NULL, "\t");

		//인자가 여러개 인 경우
		if (strchr(parsing_token, ','))
		{
			int operand_index = 0;
			sub_parsing_token = strtok(parsing_token, ",");
			while (sub_parsing_token != NULL)
			{
				token_table[token_line]->operand[operand_index] = (char *)malloc(strlen(sub_parsing_token));
				strcpy(token_table[token_line]->operand[operand_index++], sub_parsing_token);

				//operand 후, 개행문자가 나오는 경우
				if (sub_parsing_token[strlen(sub_parsing_token) - 1] == '\n')
				{
					sub_parsing_token[strlen(sub_parsing_token) - 1] = '\0';
					sub_parsing_token = strtok(NULL, ",");
				}
				else
				{
					sub_parsing_token = strtok(NULL, ",");
					parsing_token = strtok(NULL, "\t");
				}
			}
		}
		//operand인자가 1개 인 경우
		else
		{
			token_table[token_line]->operand[0] = (char *)malloc(strlen(parsing_token));
			strcpy(token_table[token_line]->operand[0], parsing_token);
			parsing_token = strtok(NULL, "\t");
		}

		//comment 부분 동적 할당  TODO : comment 가 없는 경우 ?
		token_table[token_line]->comment = (char *)malloc(strlen(parsing_token));
		strcpy(token_table[token_line]->comment, parsing_token);
		parsing_token = strtok(NULL, "\t");

#ifdef DEBUG
		printf("%s\t", token_table[token_line]->label);
		printf("%s\t", token_table[token_line]->operator);
		printf("%s\t", token_table[token_line]->operand[2]);
		printf("%s\n", token_table[token_line]->comment);
#endif
		token_line++;
	}

	return 0;
}

/* ----------------------------------------------------------------------------------
 * 설명 : 입력 문자열이 기계어 코드인지를 검사하는 함수이다. 
 * 매계 : 토큰 단위로 구분된 문자열 
 * 반환 : 정상종료 = 기계어 테이블 인덱스, 에러 < 0 
 * 주의 : 
 *		
 * ----------------------------------------------------------------------------------
 */
int search_opcode(char *str)
{
	/* add your code here */
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
	for (int i = 0; i < 5; i++)
	{
		token_parsing(input_data[i]);
	}
	return 0;
}

/* ----------------------------------------------------------------------------------
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
	/* add your code here */
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
void make_symtab_output(char *file_name)
{
	/* add your code here */
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
void make_literaltab_output(char *filen_ame)
{
	/* add your code here */
}

/* --------------------------------------------------------------------------------*
* ------------------------- 추후 프로젝트에서 사용할 함수 --------------------------*
* --------------------------------------------------------------------------------*/

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

	/* add your code here */
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
