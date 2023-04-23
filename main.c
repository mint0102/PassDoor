#include "main.h"
// Noi khai bao hang so
#define     LED     PORTD
#define     ON      1
#define     OFF     0

#define     INIT_SYSTEM        13
#define     OWNER              14
#define     INIT_OWNER         2
#define     ENTER_O            3
#define     CHECK_O            15
#define     WRONG_O            4
#define     OPTION             5
#define     SET_O              6
#define     RESET_O            7
#define     CHANGE_O           8
#define     OFF_O              9
#define     ADD                10
#define     WRONG_SET          11
#define     WRONG_RESET        12
#define     SET_NEW            16
#define     SET_DF             17


#define     USER               50
#define     INIT_USER          51
#define     ENTER_U            52
#define     CHECK_U            53
#define     WRONG_U            54
#define     UNLOCK_DOOR        55
#define     CHANGE_U           56
#define     OFF_U              57

// Noi khai bao bien toan cuc
unsigned char arrayMapOfOutput [8] = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};
unsigned char statusOutput[8] = {0,0,0,0,0,0,0,0};
// Khai bao cac ham co ban IO
void init_system(void);
void delay_ms(int value);
void OpenOutput(int index);
void CloseOutput(int index);
void TestOutput(void);
void ReverseOutput(int index);
unsigned char isButtonMotorOn();
unsigned char isButtonMotorOff();
void MotorOn();
void MotorOff();
void BaiTap_Motor();
void Test_KeyMatrix();
unsigned char search();
//Chuong trinh Password Door
unsigned char arrayMapOfNumber [16] = {1,2,3,'A',4,5,6,'B',
                                       7,8,9,'C','*',0,'E','D'};
unsigned char arrayMapOfPassword [5][4]= {
  {0,0,0,0},
  {0,0,0,0},
  {0,0,0,0},
  {0,0,0,0},
  {0,0,0,0},
};
unsigned char arrayPassword[4];
unsigned char ownpass[4] = {0, 0, 0, 0};
unsigned char setuser[5] = {0, 0, 0, 0, 0}; //vi tri co the dat them mat khau
unsigned char status = INIT_SYSTEM;
unsigned char owner = INIT_OWNER;
unsigned char user = INIT_USER;
unsigned char indexOfNumber = 0;
unsigned char index = 0;
unsigned char set_id = -1;
unsigned char user_id = -1;
unsigned char timeDelay = 0;

void App_PasswordDoor();
void Owner();
void User();
unsigned char CheckPassword();
unsigned char CheckPasswordmaster();
unsigned char isButtonNumber();
unsigned char numberValue;
unsigned char isButtonEnter();
unsigned char isButtonSet();
void UnlockDoor();
void LockDoor();

////////////////////////////////////////////////////////////////////
//Hien thuc cac chuong trinh con, ham, module, function duoi cho nay
////////////////////////////////////////////////////////////////////
void main(void)
{
	unsigned int k = 0;
	init_system();
        //TestOutput();
	while (1)
	{
            while (!flag_timer3);
            flag_timer3 = 0;
            // thuc hien cac cong viec duoi day
            scan_key_matrix();
            App_PasswordDoor();
            DisplayLcdScreen();
	}
}
// Hien thuc cac module co ban cua chuong trinh
void delay_ms(int value)
{
	int i,j;
	for(i=0;i<value;i++)
		for(j=0;j<238;j++);
}

void init_system(void)
{
    TRISB = 0x00;		//setup PORTB is output
    TRISD = 0x00;
    init_lcd();
    LcdClearS();
    //        LED = 0x00;
    init_interrupt();
    delay_ms(1000);
    init_timer0(4695);//dinh thoi 1ms sai so 1%
    init_timer1(9390);//dinh thoi 2ms
    init_timer3(46950);//dinh thoi 10ms
    SetTimer0_ms(2);
    SetTimer1_ms(10);
    SetTimer3_ms(50); //Chu ky thuc hien viec xu ly input,proccess,output
    init_key_matrix();
}

void OpenOutput(int index)
{
	if (index >= 0 && index <= 7)
	{
		LED = LED | arrayMapOfOutput[index];
	}

}

void CloseOutput(int index)
{
	if (index >= 0 && index <= 7)
	{
		LED = LED & ~arrayMapOfOutput[index];
	}
}

void ReverseOutput(int index)
{
    if (statusOutput[index]  == ON)
    {
        CloseOutput(index);
        statusOutput[index] = OFF;
    }
    else
    {
        OpenOutput(index);
        statusOutput[index] = ON;
    }
}

void TestOutput(void)
{
	int k;
	for (k=0;k<=7 ;k++ )
	{
		OpenOutput(k);
		delay_ms(500);
		CloseOutput(k);
		delay_ms(500);
	}
}
unsigned char isButtonMotorOn()
{
    if (key_code[2] == 1)
        return 1;
    else
        return 0;
}

unsigned char isButtonMotorOff()
{
    if (key_code[3] == 1)
        return 1;
    else
        return 0;
}

unsigned char isButtonSet()   //set = 14
{
    if (key_code[14] == 1)
        return 1;
    else
        return 0;
}
void App_PasswordDoor()
{
    switch (status)
    {
        case INIT_SYSTEM:
            owner = INIT_OWNER;
            user = INIT_USER;
            LcdPrintStringS(0,0,"PRESS * FOR OWN");
            LcdPrintStringS(1,0,"PRESS # FOR USER");
            // con thieu action
            if (isButtonEnter())
            {
                status = OWNER;
            }
            if (isButtonSet())
            {
                status = USER;
            }
            break;
        case OWNER:
            Owner();
            break;
        case USER:
            User();
            break;
        default:
            status = INIT_SYSTEM;
            break;
    }

}
unsigned char CheckPassword()
{
    unsigned char i,j;
    unsigned result = 1;
    for (i=0;i<5;i++)
    {
        result = 1;
        for (j=0;j<4;j++)
        {
            if (arrayPassword[j] != arrayMapOfPassword[i][j] || setuser[i] == 0)
                result = 0;
        }
        if (result == 1)
            return (i+1);
    }

}
unsigned char isButtonNumber()
{
    unsigned char i;
    for (i = 0; i<=15; i++)
        if (key_code[i] == 1)
        {
            numberValue = arrayMapOfNumber[i];
            return 1;
        }
    return 0;
}

void Owner()
{
    switch (owner)
    {
        case INIT_OWNER:
            LcdPrintStringS(0,0,"PRESS * FOR PASS");
            LcdPrintStringS(1,0,"                ");
            // con thieu action
            if (isButtonEnter())
            {
                indexOfNumber = 0;
                timeDelay = 0;
                owner = ENTER_O;
                LcdClearS();
            }
            break;
        case ENTER_O:
            LcdPrintStringS(0,0,"ENTER OWNER PASS");
            timeDelay ++;
            if (isButtonNumber())
            {
                LcdPrintStringS(1,indexOfNumber,"*");
                arrayPassword [indexOfNumber] = numberValue;
                indexOfNumber ++;
                timeDelay = 0;
            }
            if (indexOfNumber >= 4)
            {
                indexOfNumber = 0;
                owner = CHECK_O;
            }
            if (timeDelay >= 100)
                owner = INIT_OWNER;
            // con thieu dieu kien chuyen trang thai
            break;
        case CHECK_O:
            if (CheckPasswordmaster())
                owner = OPTION;
            else
                owner = WRONG_O;
            break;
        case WRONG_O:
            LcdPrintStringS(0,0,"WRONG OWNER PASS");
            LcdPrintStringS(1,0,"RE-ENTER(*) OR EXIT(#)");
            if(isButtonEnter())
            {
                LcdClearS();
                owner = ENTER_O;
            }
            LcdClearS();
                LcdPrintStringS(0,0,"EXIT  ");
                user = OFF_O;
            break;
        case OPTION:
            LcdClearS();
            LcdPrintStringS(0,0,"SETP 1  RESP 2");
            LcdPrintStringS(1,0,"CHAN 3  EXIT 4");
            if (isButtonNumber())
            {
                LcdClearS();
                if(numberValue == 1)
                {
                    LcdPrintStringS(0,0 ,"USER LIST: ");
                    for(set_id = 0; set_id <5; set_id++)
                    {
                        LcdPrintNumS(1,set_id*2 ,setuser[set_id]);
                    }       
                    owner = SET_O;
                }
                if(numberValue == 2)
                {
                    LcdPrintStringS(0,0 ,"USER LIST: ");
                    for(set_id = 0; set_id <5; set_id++)
                    {
                        LcdPrintNumS(1,set_id*2 ,setuser[set_id]);
                    } 
                    owner = RESET_O;
                }
                if(numberValue == 3)
                {
                    LcdPrintStringS(0,0,"NEW PASSWORD: ");
                    index = 0;
                    owner = CHANGE_O;
                }
                if(numberValue == 4)
                {
                    LcdClearS();
                    LcdPrintStringS(0,0,"LOG OUT");
                        owner = OFF_O;
                }
            }   
            break;
        case SET_O:
            // set user id
            set_id = -1;
            if(isButtonNumber())
            {
                LcdClearS();
                set_id = numberValue;
            }
            if(set_id != -1)
            {
                if(setuser[set_id] == 0)
                {
                    index = 0;
                    owner = ADD;
                }
                else
                    owner = WRONG_SET;
            }
            break;
        case ADD:
            LcdPrintStringS(0,0,"SET PASSWORD  ");
            if (isButtonNumber())
            {
                LcdPrintStringS(1,index,"*");
                arrayMapOfPassword[set_id][index] = numberValue;
                index ++;
            }
            if (index == 4)
            {
                LcdClearS();
                LcdPrintStringS(0,0,"SAVE PASSWORD  ");
            }
            if (isButtonEnter())
            {
                LcdClearS();
                setuser[set_id] = 1;
                set_id = -1;
                owner = OPTION;
            }
            break;  
        case WRONG_SET:
            LcdPrintStringS(0,0,"NOT EMPTY  ");
            if (isButtonEnter())
            {
                        LcdClearS();
                        set_id = -1;
                        owner = OPTION;
                    }
                    break;
            //////////////////////////////////////////////
            /////////// RESET ////////////////////////////
            //////////////////////////////////////////////
        case RESET_O:
            // set user id
            set_id = -1;
            if(isButtonNumber())
            {
                LcdClearS();
                set_id = numberValue;
            }
            if(set_id != -1)
            {
                if(setuser[set_id] == 0)
                {
                    owner = WRONG_RESET;
                }
                else
                    owner = SET_DF;
            }
            break;            
        case SET_DF:
            LcdPrintStringS(0,0,"SET_DF ");
            if(isButtonEnter())
            {
                for(index = 0; index < 4; index++)
                {
                    arrayMapOfPassword[set_id][index] = 0;
                }
                index = 0;
                setuser[set_id] = 0;
                set_id = -1;
                LcdPrintStringS(0,0,"SAVE_DF ");
                owner = OPTION;
            }
            if(isButtonSet())
            {
                set_id = -1;
                index = 0;
                owner = ADD;
            }
            break;
        case WRONG_RESET:
            LcdPrintStringS(0,0,"WRONG_RESET  ");
            if (isButtonEnter())
                owner = OPTION;
            break;
            //////////////////////////////////////////////
            /////////// CHANGE ///////////////////////////
            //////////////////////////////////////////////
        case CHANGE_O:
            if (isButtonNumber())
            {
                LcdPrintStringS(1,index,"*");
                ownpass[index] = numberValue;
                index ++;
            }
            if (index == 4)
            {
                LcdClearS();
                LcdPrintStringS(0,0,"SAVE PASSWORD  ");
            }
            if (isButtonEnter())
            {
                LcdClearS();
                index = 0;
                owner = OPTION;
            }
            break;
        case OFF_O:
            if (isButtonEnter())
                status = INIT_SYSTEM;
            break;
        default:
            owner = INIT_OWNER;
            break;                  
    }
}

void User()
{
    switch (user)
    {
        case INIT_USER:
            LcdPrintStringS(0,0,"PRESS * FOR PASS");
            LcdPrintStringS(1,0,"                ");
            // con thieu action
            if (isButtonEnter())
            {
                indexOfNumber = 0;
                timeDelay = 0;
                user = ENTER_U;
                LcdClearS();
            }
            break;
        case ENTER_U:
            LcdPrintStringS(0,0,"ENTER USER PASS");
            timeDelay ++;
            if (isButtonNumber())
            {
                LcdPrintStringS(1,indexOfNumber,"*");
                arrayPassword [indexOfNumber] = numberValue;
                indexOfNumber ++;
                timeDelay = 0;
            }
            if (indexOfNumber >= 4)
            {
                indexOfNumber = 0;
                user = CHECK_U;
            }
            if (timeDelay >= 100)
                user = INIT_USER;
            // con thieu dieu kien chuyen trang thai
            break;
        case CHECK_U:
            if (CheckPassword())
            {
                user = UNLOCK_DOOR;
            }
            else
                user = WRONG_U;
            break;
        case UNLOCK_DOOR:
            timeDelay ++;
            LcdClearS();
            LcdPrintStringS(0,0,"OPENING DOOR    ");
            UnlockDoor();
            if(isButtonSet())
            {
                LcdClearS();
                index = 0;
                user_id = search();
                LcdPrintStringS(0,0,"NEW PASSWORD: ");
                LcdPrintNumS(0,14,user_id);
                user = CHANGE_U;
            }
            if(isButtonEnter())
            {
                LcdClearS();
                LcdPrintStringS(0,0,"LOG OUT");
                if(isButtonEnter())
                {
                    user = OFF_U;
                }
            }
            // con thieu dieu kien chuyen trang thai
            break;
        case WRONG_U:
            LcdClearS();
            LcdPrintStringS(0,0,"WRONG USER PASS");
            if(isButtonEnter())
            {
                indexOfNumber = 0;
                timeDelay = 0;
                user = ENTER_U;
                LcdClearS();
            }
            if(isButtonSet())
            {
                indexOfNumber = 0;
                timeDelay = 0;
                LcdClearS();
                LcdPrintStringS(0,0,"EXIT  ");
                user = OFF_U;
            }
            break;
        case CHANGE_U:
            if (isButtonNumber())
            {
                LcdPrintStringS(1,index,"*");
                arrayMapOfPassword[user_id][index] = numberValue;
                index ++;
            }
            if (index == 4)
            {
                LcdClearS();
                LcdPrintStringS(0,0,"SAVE PASSWORD  ");
                for(index = 0; index < 4; index++)
                {
                    LcdPrintNumS(1,index*2,arrayMapOfPassword[user_id][index]);
                }
            }
            if (isButtonEnter())
            {
                LcdClearS();
                index = 0;
                user_id = -1;
                user = UNLOCK_DOOR;
            }
            break;
        case OFF_U:
            if (isButtonEnter())
                status = INIT_SYSTEM;
            break;
        default:
            user = INIT_USER;
            break;                  
    }
}
unsigned char isButtonEnter()
{
    if (key_code[12] == 1)
        return 1;
    else
        return 0;
}
void UnlockDoor()
{
    OpenOutput(0);
    //OpenOutput(4);
}
void LockDoor()
{
    CloseOutput(0);
    //CloseOutput(4);
}
unsigned char CheckPasswordmaster()
{
    unsigned char i,j;
    unsigned result = 1;
    for (i=0;i<4;i++)
    {
        if (arrayPassword[i] != ownpass[i])
            result = 0;
    }
    return result;
}
unsigned char search()
{
    unsigned char i;
    unsigned result = -1;
    for (i=0;i<5;i++)
    {
        if (arrayPassword[0] == arrayMapOfPassword[i][0] && arrayPassword[1] == arrayMapOfPassword[i][1] && arrayPassword[2] == arrayMapOfPassword[i][2] && arrayPassword[3] == arrayMapOfPassword[i][3] && setuser[i] == 1)
            result = i;
    }
    return result;
}