/** uni.h *****************************************************

NOTES :
1. UNI��field�����Q�����ȏ�ɑΉ�
2. ��s�������Ă�����ɓǂݍ��߂�
3. ��Έʒu(position)���̗p
4. load_uni �֐�����
***************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* �ǂݍ��݉\��unievent�̍ő吔 */
#define MAX_UNIEVENTS 1024

#define FLAG_ON 1
#define FLAG_OFF 0


/** global �ϐ���` ***************************************/

// unievent�\���̒�`
struct SUniEvent{
	long interval;   // ^I  �C���^�[�o��
	int key_number;    // ^K  ����
	int velocity;   // ^V  �x���V�e�B�[�i���̋����j
	int length;     // ^L  ���̒���
	int channel;    // ^W  ����
	int measure;    // ^M  ����
	int tempo;      // ^T  �e���|

	long position; // �ȊJ�n����̈ʒu�i��interval�j
};

// unievent�̔z��
struct SUniEvent unievent[MAX_UNIEVENTS + 1];

// unievent[]���ێ����Ă���Event�̐�
int n_unievents = 0;


/** init_unievents() : unievent[]�̏������p�֐��̒�` **************/
void init_unievents()
{
	int i;

	for (i = 0; i < MAX_UNIEVENTS ; i++){
		unievent[i].interval = 0;
		unievent[i].key_number = -1;
		unievent[i].velocity = 0;
		unievent[i].length = 0;
		unievent[i].channel = 1;
		unievent[i].measure = 0;
		unievent[i].tempo = 0;

		unievent[i].position = 0;
	}

	n_unievents = 0;
	return;
}



/*** unievent[].interval ����unievent[].position ���v�Z����֐��̒�` ******/
void calc_uni_position(){

	int i;
	unievent[0].position = 0;
	for (i = 1; i <= n_unievents ; i++){
		unievent[i].position
			= unievent[i].interval + unievent[i-1].position ;
	}
	return ;
}



/*** display_unievents() :  unievent[] �S�v�f�\������֐��̒�` ************/
void display_unievents(){

	int i;
	calc_uni_position(); // position�i��Ύ��Ԓl�j�̌v�Z
	for ( i = 1; i <= n_unievents ; i++){
		printf("%4d : [pos:%5d] ^I%4d ^K%3d ^V%3d ^L%4d ^W%2d ^M%3d ^T%3d\n",
			i,
			unievent[i].position,
			unievent[i].interval,
			unievent[i].key_number,
			unievent[i].velocity,
			unievent[i].length,
			unievent[i].channel,
			unievent[i].measure,
			unievent[i].tempo
		);
	}
}


/*** int set_unievent_value()  ****************************************
   n �Ԗڂ�unievent�̎w�肵���t�B�[���h��value��ݒ�

����
  int n : n �Ԗڂ�unievent
  const char fieldname[] : �t�B�[���h�̖��O
  int value : �ݒ�l

�Ԃ�l
�@����ɒl��ݒ�ł����1�A�ݒ�ł��Ȃ����0��Ԃ�

**********************************************************************/
int set_unievent_value(int n, const char *fieldname, int value ){

	/* n �� n_unievents �����傫�����́A
	�@ unievent[]�̍Ō�ɗv�f��������� */
	if (n > n_unievents){
		n_unievents++;
		printf("unievent���� %d �ɍX�V���܂����B\n",n_unievents);
        n = n_unievents;
	}

	/* �����ƃ}�b�`���������o�ϐ��ɒl���� */
	if     (strcmp(fieldname,"I") == 0){
		unievent[n].interval = (long)value;
	}
	else if(strcmp(fieldname,"K") == 0){
		unievent[n].key_number = value;
	}
	else if(strcmp(fieldname,"V") == 0){
		unievent[n].velocity = value;
	}
	else if(strcmp(fieldname,"L") == 0){
		unievent[n].length = value;
	}
	else if(strcmp(fieldname,"W") == 0){
		unievent[n].channel = value;
	}
	else if(strcmp(fieldname,"M") == 0){
		unievent[n].measure = value;
	}
	else if(strcmp(fieldname,"T") == 0){
		unievent[n].tempo = value;
	}
	else {
		printf("%s �͖���`�̃t�B�[���h�ł��B\n",fieldname);
		return 0; //  value set ���s
	}

	//printf("Set unievent[%d].%s = %d\n",n, fieldname, value);

	return 1;  // value set ����
}


/*** load_uni() *****************************************************
NOTE:  �w�肵��UNI�t�@�C�����J���Aunievent[]�ɉ��������i�[
 ���� : �ǂݍ���UNI�t�@�C���̖��O
 �Ԃ�l : ����ɓǂݍ��߂��1�A�ǂݍ��߂Ȃ����0
**********************************************************************/
int load_uni(const char *unifilename)
{
	/* ==== �ϐ��錾�� ======================================== */
	FILE *fp;

	char ch;  // UNI�t�@�C������ǂݍ��ނP�������i�[
	int number;  // UNI�̃t�B�[���h�l�̐����P�����i�[

	char fieldname[10];   // fieldname ���ꎞ�I�Ɋi�[
	int i;                // fieldname �̕������J�E���g�p
	int value = 0;        // field�l���ꎞ�I�Ɋi�[

	/* UNI�t�@�C���Ǎ��ݒ��̃f�[�^���ʂׂ̈̃t���O */
	int f_fieldname = FLAG_OFF;  // fieldname�̃t���O
	int f_value = FLAG_OFF;      // value�̃t���O

	/* ======================================================== */

	init_unievents();   // unievent[] ������
	fp = fopen(unifilename, "r");      // UNIfile���J��
	i = 0;              // fieldname �̕������J�E���g�p

	/** �� UNI�f�[�^�ǂݍ��݃��[�`�� ***********************/
	while( (ch = fgetc(fp)) != EOF ){

		switch (ch)
		{
		case '^':  // field���̓Ǎ��݊J�n�L�� ==========================/

			if ( (!f_fieldname) && (!f_value) ){
				if(n_unievents >= MAX_UNIEVENTS){
					printf("UNI�t�@�C�����[�h���ɁAUNI�C�x���g���̏���𒴂��܂����B\n");
					return 0; // �G���[�I��
				}
				n_unievents++;
			}
			/*** value�t���O��OFF. ���O�ɓǂݍ���value���Z�b�g ******/
				if(f_value == FLAG_ON){
					f_value = FLAG_OFF;
					set_unievent_value(n_unievents, fieldname, value);
				}
			/*** fieldname�t���O��ON. value�̓Ǎ��݂̏��� *************/
				f_fieldname = FLAG_ON ;
				i = 0;	// fieldname �ǂݍ��ݗp�J�E���^���Z�b�g
				break;

		case '%':  /*== field�l�̓Ǎ��݊J�n�L�� ============================*/
			/***** fieldname�t���O��OFF. field���͂����܂� ****************/
				f_fieldname = FLAG_OFF;
				fieldname[i] = '\0';  // fieldname�����
			/***** value�t���O��ON. fieldname �̓Ǎ��݂̏��� **************/
				f_value = FLAG_ON ;
				value = 0; // field�l�̏�����
				break;

		case '\n':   /*== ���s�R�[�h�B�P��unievent�̋�؂� ===============*/
			/****** value�t���O��ON�Ȃ�A���O�ɓǂݍ���value���Z�b�g *****/
				if(f_value == FLAG_ON){
					set_unievent_value(n_unievents, fieldname, value);
					f_value = FLAG_OFF;
				}
				break;

		default:	// === ������ ^ % \n �ȊO�̎� ============================/
			// fieldname�t���O��ON�Ȃ�fieldname�Ƃ��ēǂݍ��� --------------
			if ( f_fieldname && (! f_value) )
			{
				fieldname[i] = ch;
				i++;
			}
			// value�t���O��ON�Ȃ�value�Ƃ��ēǂݍ��� ----------------------
			else if ( (! f_fieldname) && f_value)
			{
				number = ch - '0';  // ����(char)�𐔒l(int)�ɕϊ�

				// �ǂݍ��񂾕����������łȂ���΃G���[
				if ( number < 0 || number > 9 ){
					printf("\nfield�l�ɕ��� %c ���܂܂�Ă��܂����B\n",ch);
					printf("UNI�f�[�^��field�l�ɂ͐����ȊO�L�q���Ȃ��ł��������B\n");
					return 0;   // �G���[�I��
				}
				value = value * 10 + number ;  // value�l�ɐ��l���P���ǉ�
			}
			 // fieldname��value���t���O�������Ă��Ȃ� or ���������Ă���ꍇ�͕s��----
			else{
				printf("UNI�t�@�C���̃t�H�[�}�b�g���s���ł��B\n");
				printf("�P�s�̎n�߂͕K�� ^�t�B�[���h�� �ɂ��Ă��������B\n");
				return 0;  // �G���[�I��
			}
			break;
		}

	}

	// �Ō�ɉ��s�����������ׂɓǂݎc�����f�[�^���i�[
	if(f_value == FLAG_ON){
		f_value = FLAG_OFF;
		set_unievent_value(n_unievents, fieldname, value);
	}
    /*** �� UNI�f�[�^�ǂݍ��ݏI�� ********************/

	calc_uni_position();  // unievent�̐�Ύ���(position)���v�Z

	fclose(fp);	  // UNI�t�@�C�������
	return 1;     // ����I���B1��Ԃ�
}


/*** EOF *********************************************************************/
