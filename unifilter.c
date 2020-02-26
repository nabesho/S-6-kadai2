/*** unifilter.c *****************************************
 NOTE :
**********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "uni.h"

void MoveKey(int start, int end, int value){ //�ڒ� ����m�F�ς�
	if (value * value > 144){
		printf("�ڒ������s�K�ł��B");
		exit(-1);
	}
	int i,j;
	for (i = start; i <= end; i++){
		if (unievent[i].key_number != -1){
			j = unievent[i].key_number + value;
			//�ڒ��悪���Ղ̊O�������ꍇ�ɃI�N�����A�I�N�グ���s��
			if (j < 0){
				j += 12;
			}
			else if (j>127){
				j -= 12;
			}
			unievent[i].key_number = j;
		}
	}

	return;
}

void CVelocity( int start, int end, int value){ //����ύX ����m�F�ς�
	if (value <10 || value > 1000){
		printf("����ύX�����s�K�ł��B");
		exit(-1);
	}

	int i;
	for (i = start; i <= end; i++){
		unievent[i].velocity = unievent[i].velocity *value / 100;
	}
	return;
}

void CTempo(int start, int end, int value){ //�e���|�ύX ����m�F�ς�
	if (value <10 || value > 1000){
		printf("�e���|�̕ύX�����s�K�ł��B");
		exit(-1);
	}

	int i;
	for (i = start; i <= end; i++){
		unievent[i].length = unievent[i].length *100/value;
		unievent[i + 1].interval = unievent[i + 1].interval  *100/value;
	}
	return;
}

void Staccato(int start, int end){ //�X�^�b�J�[�g ����m�F�ς�
	int i;
	for (i = start; i <= end; i++){
		unievent[i].length = unievent[i].length / 2;
	}
	return;
}

void Legato(int start, int end){ //���K�[�g ����m�F�ς�
	int i;
	for (i = start; i <= end; i++){
		unievent[i].length += unievent[i].length  / 5;
	}
	return;
}

void Delete(int start, int end){ //���� ����m�F�ς�
	int i;
	int k = start - 1; //k��Delete�͈͓̔��ł��c��C�x���g�ɑ΂��鏈�����s�����߂̂���
	for (i = start; i <= end; i++){
		if (unievent[i].measure == 0){
			unievent[i].interval = 0;
			unievent[i].key_number = -1;
			unievent[i].velocity = 0;
			unievent[i].length = 0;
			unievent[i].channel = 1;
			unievent[i].position = 0;
			unievent[i].tempo = 0;
		}
		else{
			unievent[i].interval = unievent[i].position - unievent[k].position;
			k = i; //unievent[start-1]����_�Ƃ��āADelete���ł��c��C�x���g�ɑ΂��āAposition�̍�����interval�����蓖�ĂēK�؂ȏꏊ�ɋx���Ə��ߐ������B
		}


	}
	if (end < n_unievents){
		unievent[end + 1].interval = unievent[end + 1].position - unievent[k].position;
	}
}

void write_uni(char *filename){ //�������� //����m�F�ς�

	FILE *fp;

	int i;

	if ((fp = fopen(filename, "w")) == NULL){
		exit(-1);
	}
	else{
		for (i = 1; i <= n_unievents; i++){
			if (unievent[i].key_number != -1 || unievent[i].measure != 0){ //�C�x���g�̗L���̔��ʂ̂���keynumber��measure���t���O�Ƃ��ėp����
				printf("^I%%%d", unievent[i].interval);
				if (unievent[i].key_number == -1){
					printf("^M%%%d\n", unievent[i].measure);
				}
				else{
					printf("^K%%%d^V%%%d", unievent[i].key_number, unievent[i].velocity);
					printf("^L%%%d^W%%%d\n", unievent[i].length, unievent[i].channel);
				}
			}
		}

		for (i = 1; i <= n_unievents; i++){
			if (unievent[i].key_number != -1 || unievent[i].measure != 0){ //�C�x���g�̗L���̔��ʂ̂���keynumber��measure���t���O�Ƃ��ėp����
				fprintf(fp, "^I%%%d", unievent[i].interval);
				if (unievent[i].key_number == -1){
					fprintf(fp, "^M%%%d\n", unievent[i].measure);
				}
				else{
					fprintf(fp, "^K%%%d^V%%%d", unievent[i].key_number, unievent[i].velocity);
					fprintf(fp, "^L%%%d^W%%%d\n", unievent[i].length, unievent[i].channel);
				}
			}
		}
		fclose(fp);
	}
}




/*** main �֐� *****************************************/

void main(int argc, char *argv[])
{
	//printf("%d",argc );
	if (argc <5){
		printf("�s�K�؂ȓ��͂ł��B\n");
		exit(-1);
	}
	//�t�@�C���̓ǂݍ���
	if (!load_uni(argv[1])){
		printf("�t�@�C���̓ǂݍ��݂Ɏ��s���܂����B\n");
		exit(-1);
	}


	//��Ƃ��s���ӏ�(�n�܂�ƏI���)��ǂݍ���
	int eventStart = (int)(strtol(argv[3],NULL,10)) ;
	int eventEnd = (int)(strtol(argv[4], NULL, 10));
	if (eventStart < 1 || eventEnd > 1024 || eventEnd - eventStart < 0){
		printf("���͂��ꂽ�͈͂��s�K�؂ł��B");
	}
	//printf("start = %d,end = %d", eventStart, eventEnd);
	int value;

	//�I�v�V�����ɂ���ďꍇ����������
	switch (*argv[2]){
		case 'm': //�ڒ��I�v�V����
			if (argc <6){
				printf("�ڒ����������͂ł��B\n");
				exit(-1);
			}
			value = (int)(strtol(argv[5], NULL, 10));
			//printf("%d", value);
			MoveKey(eventStart, eventEnd, value);
			break;
		case 'v': //����ύX�I�v�V����
			if (argc <6){
				printf("����̕ύX���������͂ł��B\n");
				exit(-1);
			}
			value = (int)(strtol(argv[5], NULL, 10));
			CVelocity(eventStart, eventEnd, value);
			break;
		case 't': //���x�ύX�I�v�V����
			if (argc <6){
				printf("���x�̕ύX���������͂ł��B\n");
				exit(-1);
			}
			value = (int)(strtol(argv[5], NULL, 10));
			CTempo(eventStart, eventEnd, value);
			break;
		case 's': //�X�^�b�J�[�g�I�v�V����
			Staccato(eventStart, eventEnd);
			break;
		case 'l': //���K�[�g�I�v�V����
			Legato(eventStart, eventEnd);
			break;
		case 'd': //�폜�I�v�V����
			Delete(eventStart, eventEnd);
			break;


	}

	write_uni(argv[1]);

	printf("���삪�������܂����B\n");



	//display_unievents(); // UniEvent[]�̊e�v�f��\��
}



/*******************************************************/
