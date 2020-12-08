#include <msp430f5529.h>

#define TRIGGER_PIN BIT6   // P1.6
#define ECHO_PIN BIT3  // P1.3

//definizione PIN per display settesegmenti
#define LED_B BIT1
#define LED_C BIT2
#define LED_D BIT3
#define LED_E BIT4
#define LED_F BIT5
#define LED_G BIT6

#define LED1 BIT2
#define LED2 BIT4
#define LED3 BIT5
#define LED4 BIT6

// definizione numeri display
#define Number_0 (LED_B + LED_C + LED_D + LED_E +LED_F)
#define Number_1 (LED_B + LED_C)
#define Number_2 (LED_B + LED_D + LED_G + LED_E)
#define Number_3 (LED_B + LED_C + LED_D + LED_G)
#define Number_4 (LED_B + LED_C + LED_F + LED_G)
#define Number_5 (LED_C + LED_D + LED_F + LED_G)
#define Number_6 (LED_C + LED_D  + LED_F + LED_G + LED_E)
#define Number_7 (LED_B + LED_C)
#define Number_8 (LED_B + LED_C + LED_D + LED_F + LED_G + LED_E)
#define Number_9 (LED_B + LED_C + LED_D + LED_F + LED_G)

//valore di default inizale conversione ADC
unsigned int ADCValue = 30;

int flag_mode = 1;

// funzoine display 7 segmenti
void displaySetteSegmentiShow(int input){

        switch(input)
        {

        case 0:
                P7OUT = BIT0;
                P6OUT = Number_0;
                break;
        case 1:
                P7OUT = 0;
                P6OUT = Number_1;
                break;
        case 2:
                P7OUT = BIT0;
                P6OUT = Number_2;
                break;
        case 3:
                P7OUT = BIT0;
                P6OUT = Number_3;
                break;
        case 4:
                P7OUT = 0;
                P6OUT = Number_4;
                break;
        case 5:
                P7OUT = BIT0;
                P6OUT = Number_5;
                break;
        case 6:
                P7OUT = BIT0;
                P6OUT = Number_6;
                break;
        case 7:
                P7OUT = BIT0;
                P6OUT = Number_7;
                break;
        case 8:
                P7OUT = BIT0;
                P6OUT = Number_8;
                break;
        case 9:
                P7OUT = BIT0;
                P6OUT = Number_9;
                break;
        case 11:
                // caso speciale utilizzato nel settaggio della soglia
                P7OUT = 0;
                P6OUT = LED_G;
                break;
        default:
                break;

        }

}


void main(void)
{


    int durata_impulso_echo;
    float distance_cm;


    WDTCTL = WDTPW + WDTHOLD;
    P1DIR = TRIGGER_PIN+BIT0;

    P1REN |= BIT1;                            // abilitazione resistenza interna di P1.1
    P1OUT |= BIT1;                            // resistenza di pull-up P1.1
    P1IES &= ~BIT1;                           // setto interrut sull edge Lo/Hi di P1.1
    P1IFG &= ~BIT1;                           // P1.1 IFG cleared
    P1IE |= BIT1;                             // ablitazione interrupt P1.1



    P3DIR = LED1+LED2+LED3+LED4;              // setto in out pin controllo cifre di
    P1IN = ECHO_PIN;                          // setto out pin echo per sensore hc-sr04
    P7DIR = BIT0;                             // out led A display
    P6DIR = LED_B + LED_C + LED_D + LED_F + LED_G + LED_E;       // out pin display 7 segmenti
    P8DIR = BIT1;                             // out led soglia

    TA1CTL = TASSEL_2 + ID_0 + MC_2;          // SMCLK, no div ,continuous

    __bis_SR_register( GIE );                 // Abilita interrupt globali

    ADC12CTL0 = ADC12SHT02 + ADC12ON;         // sampling time, ADC12 on vref defualt 3,3 (vcc)
    ADC12CTL1 = ADC12SHP;                     // uso sampling timer
    ADC12CTL0 |= ADC12ENC;
    P6SEL |= 0x01;                            // setto P6.0 come ADC

    while(1)
    {

        if(flag_mode == 1){
            P1OUT &= ~BIT0;
            TA1R = 0;                          // Reset del timer prima del'inizio dell' impulso di trig

                                               // Send a 20us trigger pulse
            P1OUT |= TRIGGER_PIN;              // impulso trigger
            __delay_cycles(15);                // 20us delay
            P1OUT &= ~TRIGGER_PIN;             // fine trigger

            // misurazione echo
            while ((P1IN & ECHO_PIN) == 0);    // inizio misurazione echo
            TA1R = 0;                          // reset timer

            while ((P1IN & ECHO_PIN) > 0);     // aspetto risposta da echo
            durata_impulso_echo = TA1R;        // assegnazione timer a echo_pulse
            distance_cm = 0.0171 * durata_impulso_echo;      // conversione in cm
        }
        else{
            P1OUT |= BIT0;
            ADC12CTL0 |= ADC12SC;              // inizio sampling/conversion

            while (ADC12CTL1 & ADC12BUSY);     // aspetto fine conversione
            ADCValue = ADC12MEM0/10;
            distance_cm = ADCValue;
        }


        //
        int display_1 = (((int)distance_cm) % 1000) /100;
        int display_2 = (((int)distance_cm) % 100) /10;
        int display_3 = ((int)distance_cm) % 10;

        float dec = distance_cm*10;
        int display_4 = ((int)dec) % 10;

        if(distance_cm < ADCValue)
        {
            P8OUT |= BIT1;
        }
        else
        {
            P8OUT &= ~BIT1;
        }

        int i=1;
        for(;i<5;i++)
        {

            switch(i){
            case 1:
                P3OUT &= ~LED1;
                P3OUT |= LED2;
                P3OUT |= LED3;
                P3OUT |= LED4;
                displaySetteSegmentiShow(display_1);
                break;

            case 2:
                P3OUT &= ~LED2;
                P3OUT |= LED1;
                P3OUT |= LED3;
                P3OUT |= LED4;
                displaySetteSegmentiShow(display_2);
                break;

            case 3:
                P3OUT &= ~LED3;
                P3OUT |= LED2;
                P3OUT |= LED1;
                P3OUT |= LED4;
                displaySetteSegmentiShow(display_3);
                break;

            case 4:
                if(flag_mode == 0){
                P3OUT &= ~LED4;
                P3OUT |= LED2;
                P3OUT |= LED3;
                P3OUT |= LED1;
                displaySetteSegmentiShow(11);
                }
                break;

            default:
                break;

            }
        }
    }
}


// Port 1 interrupt service routine
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{
    switch( __even_in_range( P1IV, P1IV_P1IFG7 )) {
    case P1IV_P1IFG1:
        if(flag_mode == 0)
        {
            flag_mode =1;
        }
        else{
            flag_mode =0;
        }
        break;
    default:   _never_executed();

    }
}

