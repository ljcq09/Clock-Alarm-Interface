#include "mbed.h"
#include "C12832.h"

/*
Name:Lenin Jhoan Cruz Quishpe
IdNumber:10131333
Group 4
*/

typedef enum _Screen{
        PrincipalScreen,
        Menu,
        SetTimeScreen,
        SetMonthDay,
        SetYear,
        SetAlarm,
        DisplaySec,
        PlayMario,
        SetSeconds
        }Screen;


class DisplayAll {
        private:
            C12832* lcd;
            Ticker tick;
        public:
            DisplayAll (C12832* LCD): lcd(LCD){};
            
            void Clear(){lcd->cls();};                                          
            void Time(int hr, int min, int sc, int dy, int mth, int yr /*int ahour, int amin*/){ 
                
                
                                            // This function prints my counter in my Screen PrincipalScreen;
                lcd->locate(28,1); lcd->printf("Time        Date");
                lcd->locate(18,10); lcd->printf("%02d:%02d:%02d    %02d/%02d/%d ",hr,min,sc,dy,mth,yr);
                lcd->locate(15,22); lcd->printf("\"Press fire for Menu\"");
            }  
            void ChangeTime(int HR, int MIN){  
                                            // This function prints the _Screen SetTimeScreen  
                lcd->locate(5,3); lcd->printf("Set Hour       Set Minute"); 
                lcd->locate(10,12); lcd->printf("  %02d              %02d",HR,MIN);  
                lcd->locate(12,23); lcd->printf("\"Press fire to continue\"");  
            }
            void PrintSetAlarm(int Ahour, int Amin){
            
                lcd->locate(45,1); lcd->printf("Set Alarm");
                lcd->locate(10,12); lcd->printf("  Hr=%02d        Min=%02d",Ahour,Amin); 
                lcd->locate(12,23); lcd->printf("\"Press fire to continue\"");    
            }       
            void PrintMenu () {                                                 //This just print the text for my _Screen Menu
                Clear();
                lcd->locate(1,1); lcd->printf("Up: Alarm");
                lcd->locate(1,12); lcd->printf("Left: Time");
                lcd->locate(65,1); lcd->printf("Down: Counter");
                lcd->locate(65,12); lcd->printf("Right: Date");
                lcd->locate(40,23); lcd->printf("Fire: Back");
            }
            void PrintSecCounter (int scs){
                lcd->locate(5,1); lcd->printf("Seconds since Clock is on");  
                lcd->locate(50,12); lcd->printf("%06d",scs);
                lcd->locate(12,23); lcd->printf("\"Press fire to continue\"");   
            }
            void PrintSetYear (int years){
                lcd->locate(5,1); lcd->printf("Set Years    \"Pot2\"");  
                lcd->locate(50,12); lcd->printf("%02d",years);
                lcd->locate(12,23); lcd->printf("\"Press fire to continue\"");   
            }
            void PrintSetMonthDay (int dy, int mot){
                lcd->locate(5,3); lcd->printf("Set Day       Set Month"); 
                lcd->locate(10,12); lcd->printf("   %02d        %02d",dy,mot); 
                lcd->locate(12,23); lcd->printf("\"Press fire to continue\""); 
            }
            
            void PrintMario(){
                Clear();
               
                lcd->locate(1,12); lcd->printf("Just some fun");
                lcd->locate(12,23); lcd->printf("\"Press fire to continue\""); 
                //mySpeaker->MarioTheme();
            }
            void PrintSeconds(int secondsset){
                
                lcd->locate(5,1); lcd->printf("Set Seconds    \"Pot2\"");  
                lcd->locate(50,12); lcd->printf("%02d",secondsset);
                lcd->locate(12,23); lcd->printf("\"Press fire to continue\""); 
            }
            
};


class Potentiometer {
    private:
        AnalogIn inputSignal;
        float VDD, currentSamplePC, currentSampleVolts;
    public:
        Potentiometer(PinName pin, float v) : inputSignal(pin), VDD(v) {}
        
        float amplitudeVolts(void) { return (inputSignal.read()*VDD); }
        float amplitudePC(void) { return inputSignal.read(); }
        void sample(void) {
             currentSamplePC = inputSignal.read();
             currentSampleVolts = currentSamplePC * VDD;}
        float getCurrentSampleVolts(void) { return currentSampleVolts; }
        float getCurrentSamplePC(void) { return currentSamplePC; }
};

class RotationHM {
    private:
        DisplayAll* disp;
        Potentiometer* potleft;
        Potentiometer* potrigth;
        const float smin, smax, Hmin, Hmax, Mmin, Mmax, Smin, Smax, Dmin, Dmax, Momin, Momax, Ymin, Ymax; 
        float gdtH, gdtM, gdtS, gdtD, gdtMo, gdtY, incptH, incptM, incptS, incptD, incptMo, incptY;
        int SaveHour, SaveMin, SaveSec, SaveDay, SaveMonth, SaveYear;
        int SaveAlarmHour, SaveAlarmMin;
        Timeout to;
        float calc_gdt(float min, float max) { return (max-min)/(smax-smin); }
        float calc_incpt(float max, float gdt) { return (max - gdt*smax);}
    public:
        RotationHM(DisplayAll* dis, Potentiometer* l, Potentiometer* r, float xmn, float xmx, float Hourmin, float Hourmax, float Minmin, float Minmax, float Secmin,float Secmax, float Dimin, float Dimax, float Monmin, float Monmax, float Yemin, float Yemax)
        : disp(dis), potleft(l), potrigth(r), smin(xmn), smax(xmx), Hmin(Hourmin), Hmax(Hourmax), Mmin(Minmin), Mmax(Minmax), Smin(Secmin),Smax(Secmax),Dmin(Dimin),Dmax(Dimax), Momin(Monmin), Momax(Monmax), Ymin(Yemin), Ymax(Yemax) {
        gdtH = calc_gdt(Hmin, Hmax);
        gdtM = calc_gdt(Mmin, Mmax);
        gdtS = calc_gdt(Smin, Smax);
        
        gdtD = calc_gdt(Dmin, Dmax);
        gdtMo = calc_gdt(Momin, Momax);
        gdtY = calc_gdt(Ymin, Ymax);
        
        incptH = calc_incpt(Hmax, gdtH);
        incptM = calc_incpt(Mmax, gdtM);
        incptS = calc_incpt(Smax, gdtS);
        
        incptD = calc_incpt(Dmax, gdtD);
        incptMo = calc_incpt(Momax, gdtMo);
        incptY = calc_incpt(Ymax, gdtY);
        
        };

        void updateRate(void) {
             SaveHour = (gdtH*potleft->getCurrentSampleVolts() + incptH);
             SaveMin = (gdtM*potrigth->getCurrentSampleVolts() + incptM);
             
             
             disp->ChangeTime (SaveHour, SaveMin);
             to.attach(callback(this, &RotationHM::updateRate), 0.1);
        }
       void updateRateAlarm (void){
             SaveAlarmHour = (gdtH*potleft->getCurrentSampleVolts() + incptH);
             SaveAlarmMin = (gdtM*potrigth->getCurrentSampleVolts() + incptM);
             disp->PrintSetAlarm (SaveAlarmHour, SaveAlarmMin);  
             to.attach(callback(this, &RotationHM::updateRateAlarm), 0.1);
        }  
        
       void updateSeconds (void){
            SaveSec = (gdtS*potrigth->getCurrentSampleVolts() + incptS);
            disp->PrintSeconds (SaveSec); 
            to.attach(callback(this, &RotationHM::updateSeconds), 0.1);
        }
        
       void  updateYears (void){
            SaveYear = (gdtY*potrigth->getCurrentSampleVolts() + incptY);
            disp->PrintSetYear (SaveYear);
            to.attach(callback(this, &RotationHM::updateYears), 0.1);
        }
        
        void updateMonthDay(void){
            SaveDay = (gdtD*potleft->getCurrentSampleVolts() + incptD);
            SaveMonth = (gdtMo*potrigth->getCurrentSampleVolts() + incptMo);
             disp->PrintSetMonthDay (SaveDay, SaveMonth);  
             to.attach(callback(this, &RotationHM::updateMonthDay), 0.1);
         }
         
        int getshour (){return SaveHour;};
        int getsmin () {return SaveMin;};
        int getssec(){return SaveSec;};
        
        int getsday () {return SaveDay;};
        int getsmonth () {return SaveMonth;};
        int getsyear () {return SaveYear;};
        
        int getsAlarmhour (){return SaveAlarmHour;};
        int getsAlarmmin () {return SaveAlarmMin;};
        
        void hideSaving (void){
            //dips->Clear();
            to.detach();
            };
}; 

class Speaker{
    private:
    PwmOut _pin;
    Ticker tick;
    int hour, minute, second, Ahour, Aminute, Asecond;
    public:
        Speaker(PinName pin) : _pin(pin) {};
                void PlayNote(float frequency, float duration, float volume) {
                    _pin.period(1.0f/frequency);
                    _pin = volume/2.0f;
                    wait(duration);
                    _pin = 0.0;
                }
                
        void MarioTheme (void){
                 PlayNote(1318.51, 0.16, 0.71);wait(0.005);PlayNote(1318.51, 0.16, 0.71);wait(0.005);
                 PlayNote(1318.51, 0.16, 0.71);wait(0.005);PlayNote(1046.502, 0.16, 0.71);wait(0.005);
                 PlayNote(1318.51, 0.16, 0.71);wait(0.005);PlayNote(1567.932, 0.36, 0.71);wait(0.25);
                 PlayNote(783.991, 0.41, 0.71);wait(0.6); 
               
                 PlayNote(523.251, 0.41, 0.71);wait(0.2); PlayNote(783.991, 0.35, 0.71);wait(0.1);
                 PlayNote(659.255, 0.16, 0.71);wait(0.005); PlayNote(880, 0.16, 0.71);wait(0.005);
                 PlayNote(987.767, 0.16, 0.71);wait(0.005); PlayNote(932.328, 0.16, 0.71);wait(0.005);
                 PlayNote(880, 0.16, 0.71);wait(0.005); PlayNote(783.991, 0.16, 0.71);wait(0.01);
                
                 PlayNote(1318.51, 0.16, 0.71);wait(0.005); PlayNote(1567.932, 0.16, 0.71);wait(0.005);
                 PlayNote(1760, 0.16, 0.71);wait(0.0005); PlayNote(1396.913, 0.16, 0.71);wait(0.0005);
                 PlayNote(1567.932, 0.16, 0.71);wait(0.0005); PlayNote(1318.51, 0.16, 0.71);wait(0.005);
                 PlayNote(1046.502, 0.14, 0.71);wait(0.00005); PlayNote(1174.659, 0.14, 0.71);wait(0.00005);
                 PlayNote(987.767, 0.14, 0.71);wait(0.5);
                
                 PlayNote(523.251, 0.41, 0.71);wait(0.2); PlayNote(783.991, 0.35, 0.71);wait(0.1);
                 PlayNote(659.255, 0.16, 0.71);wait(0.005); PlayNote(880, 0.16, 0.71);wait(0.005);
                 PlayNote(987.767, 0.16, 0.71);wait(0.005); PlayNote(932.328, 0.16, 0.71);wait(0.005);
                 PlayNote(880, 0.16, 0.71);wait(0.005); PlayNote(783.991, 0.16, 0.71);wait(0.01);
               
                 PlayNote(1318.51, 0.16, 0.71);wait(0.005); PlayNote(1567.932, 0.16, 0.71);wait(0.005);
                 PlayNote(1760, 0.16, 0.71);wait(0.0005); PlayNote(1396.913, 0.16, 0.71);wait(0.0005);
                 PlayNote(1567.932, 0.16, 0.71);wait(0.0005); PlayNote(1318.51, 0.16, 0.71);wait(0.005);
                 PlayNote(1046.502, 0.14, 0.71);wait(0.00005); PlayNote(1174.659, 0.14, 0.71);wait(0.00005);
                 PlayNote(987.767, 0.14, 0.71);wait(0.00005);
            
        }
        
        void BeepAlarm (void){
                 PlayNote(969.0, 0.5, 0.5);PlayNote(800.0, 0.5, 0.5);
                 PlayNote(969.0, 0.5, 0.5);PlayNote(800.0, 0.5, 0.5);
                 PlayNote(969.0, 0.5, 0.5);PlayNote(800.0, 0.5, 0.5);
                 PlayNote(969.0, 0.5, 0.5);PlayNote(800.0, 0.5, 0.5);
                 PlayNote(969.0, 0.5, 0.5);PlayNote(800.0, 0.5, 0.5);
                 PlayNote(969.0, 0.5, 0.5);PlayNote(800.0, 0.5, 0.5);
        }
        
        void Chime (void){
                PlayNote(8156, 0.20, 0.81);wait(0.02);PlayNote(8156, 0.20, 0.81);wait(0.02);  
        }
        
};

class Joystick {
    private:
        DigitalIn up, down, left, right, fire;  
        
    public:
        Joystick(PinName u, PinName d, PinName l, PinName r, PinName f):up(u),down(d),left(l),right(r),fire(f){} // constructor
    bool upPressed(void){return up;}                                            //functions
    bool downPressed(void){return down;}
    bool leftPressed(void){return left;}
    bool rightPressed(void) {return right;}
    bool firePressed(void) {return fire;}
}; 
    

class BasicClock {
        private:
            int Dhour, Dmin, Dsec, Dday, Dmonth, Dyear, DAhour, DAmin, DAsec, SecOnly;
             Speaker* spk;
             Joystick* joy;         
        public:
            BasicClock (int dhour, int dmin, int dsec, int dday, int dmonth, int dyear, int seconly, Speaker* SPK, Joystick* JOY)
            : Dhour(dhour), Dmin(dmin), Dsec(dsec), Dday(dday), Dmonth(dmonth), Dyear(dyear), SecOnly(seconly),spk(SPK),joy(JOY){}
                            
            void Counter (void){
               SecOnly++;
               if (Dyear % 4 == 0){
                        if (Dyear % 100 == 0){
                            if (Dyear % 400 == 0){// " is a leap year.";
                                if (Dmonth==2){
                                    if(Dhour==DAhour && Dmin==DAmin && Dsec<2){
                                        while(joy->downPressed()==0){spk->MarioTheme(); if(joy->downPressed()==1){break;}}
                                        Dsec=Dsec+2;
                                    }
                                      Dsec++;   
                                    if(Dsec==60){Dmin++;Dsec=0;}if(Dmin==60){Dhour++;Dmin=0;spk->Chime();}if(Dhour==24){Dday++;Dsec=0;Dmin=0;Dhour=0;}
                                    if(Dday==30){Dmonth++;Dday=1;Dsec=0;Dmin=0;Dhour=0;}
                                }
                                if ((Dmonth==4) || (Dmonth==6) || (Dmonth==9) || (Dmonth==11)){
                                        if(Dhour==DAhour && Dmin==DAmin && Dsec<2){
                                        while(joy->downPressed()==0){spk->MarioTheme(); if(joy->downPressed()==1){break;}}
                                        Dsec=Dsec+2;
                                }
                                    Dsec++;
                                    if(Dsec==60){Dmin++;Dsec=0;}if(Dmin==60){Dhour++;Dmin=0;spk->Chime();}if(Dhour==24){Dday++;Dsec=0;Dmin=0;Dhour=0;}
                                    if(Dday==31){Dmonth++;Dday=1;Dsec=0;Dmin=0;Dhour=0;}
                                }
                                if ((Dmonth == 1) || (Dmonth == 3) || (Dmonth == 5) || (Dmonth == 7) || (Dmonth == 9) || (Dmonth ==12)){
                                    if(Dhour==DAhour && Dmin==DAmin && Dsec<2){
                                        while(joy->downPressed()==0){spk->MarioTheme(); if(joy->downPressed()==1){break;}}
                                        Dsec=Dsec+2;}
                                    Dsec++;
                                    if(Dsec==60){Dmin++;Dsec=0;}if(Dmin==60){Dhour++;Dmin=0;spk->Chime();}if(Dhour==24){Dday++;Dsec=0;Dmin=0;Dhour=0;}
                                    if(Dday==32){Dmonth++;Dday=1;Dsec=0;Dmin=0;Dhour=0;}}
                                if (Dmonth == 13){Dyear=1;Dmonth=1;Dday=1;Dsec=0;Dmin=0;Dhour=0;
                                }
                            }    
                            else{//" is not a leap year."; 
                                if (Dmonth==2){if(Dhour==DAhour && Dmin==DAmin && Dsec<2){
                                        while(joy->downPressed()==0){spk->MarioTheme(); if(joy->downPressed()==1){break;}}
                                        Dsec=Dsec+2;
                                    }Dsec++;
                                    if(Dsec==60){Dmin++;Dsec=0;}
                                    if(Dmin==60){Dhour++;Dmin=0;spk->Chime();}
                                    if(Dhour==24){Dday++;Dsec=0;Dmin=0;Dhour=0;}
                                    if(Dday==29){Dmonth++;Dday=1;Dsec=0;Dmin=0;Dhour=0;}
                                }
                                if ((Dmonth == 4) || (Dmonth == 6) || (Dmonth == 9) || (Dmonth == 11)){
                                    if(Dhour==DAhour && Dmin==DAmin && Dsec<2){
                                        while(joy->downPressed()==0){spk->MarioTheme(); if(joy->downPressed()==1){break;}}
                                        Dsec=Dsec+2;
                                    }Dsec++;
                                    if(Dsec==60){Dmin++;Dsec=0;}if(Dmin==60){Dhour++;Dmin=0;spk->Chime();}if(Dhour==24){Dday++;Dsec=0;Dmin=0;Dhour=0;}
                                    if(Dday==31){Dmonth++;Dday=1;Dsec=0;Dmin=0;Dhour=0;}
                                }
                                if (Dmonth == 1 || Dmonth == 3 || Dmonth == 5 || Dmonth == 7 || Dmonth == 9 || Dmonth ==12){
                                    
                                    if(Dhour==DAhour && Dmin==DAmin && Dsec<2){
                                        while(joy->downPressed()==0){spk->MarioTheme(); if(joy->downPressed()==1){break;}}
                                        Dsec=Dsec+2;
                                    }Dsec++;
                                    if(Dsec==60){Dmin++;Dsec=0;}if(Dmin==60){Dhour++;Dmin=0;spk->Chime();}if(Dhour==24){Dday++;Dsec=0;Dmin=0;Dhour=0;}
                                    if(Dday==32){Dmonth++;Dday=1;Dsec=0;Dmin=0;Dhour=0;}
                                }
                                if (Dmonth == 13){Dyear=1;Dmonth=1;Dday=1;Dsec=0;Dmin=0;Dhour=0;
                                }
                            }
                        }
                        else{//" is a leap year.";
                            if (Dmonth==2){
                                if(Dhour==DAhour && Dmin==DAmin && Dsec<2){while(joy->downPressed()==0){spk->MarioTheme(); if(joy->downPressed()==1){break;}}
                                        Dsec=Dsec+2;
                                    }Dsec++;
                                if(Dsec==60){Dmin++;Dsec=0;}
                                if(Dmin==60){Dhour++;Dmin=0;spk->Chime();}
                                if(Dhour==24){Dday++;Dsec=0;Dmin=0;Dhour=0;}
                                if(Dday==30){Dmonth++;Dday=1;Dsec=0;Dmin=0;Dhour=0;}
                                }
                            if (Dmonth == 4 || Dmonth == 6 || Dmonth == 9 || Dmonth == 11){
                                
                                if(Dhour==DAhour && Dmin==DAmin && Dsec<2){
                                        while(joy->downPressed()==0){spk->MarioTheme(); if(joy->downPressed()==1){break;}}
                                        Dsec=Dsec+2;
                                    }Dsec++;
                                if(Dsec==60){Dmin++;Dsec=0;}
                                if(Dmin==60){Dhour++;Dmin=0;spk->Chime();}
                                if(Dhour==24){Dday++;Dsec=0;Dmin=0;Dhour=0;}
                                if(Dday==31){Dmonth++;Dday=1;Dsec=0;Dmin=0;Dhour=0;}
                                }
                            if (Dmonth == 1 || Dmonth == 3 || Dmonth == 5 || Dmonth == 7 || Dmonth == 9 || Dmonth ==12){
                                
                                if(Dhour==DAhour && Dmin==DAmin && Dsec<2){
                                        while(joy->downPressed()==0){spk->MarioTheme(); if(joy->downPressed()==1){break;}}
                                        Dsec=Dsec+2;
                                    }Dsec++;
                                if(Dsec==60){Dmin++;Dsec=0;}
                                if(Dmin==60){Dhour++;Dmin=0;spk->Chime();}
                                if(Dhour==24){Dday++;Dsec=0;Dmin=0;Dhour=0;}
                                if(Dday==32){Dmonth++;Dday=1;Dsec=0;Dmin=0;Dhour=0;}
                                }
                            if (Dmonth == 13){Dyear=1;Dmonth=1;Dday=1;Dsec=0;Dmin=0;Dhour=0;
                            }
                        }
                    }
                    else{//" is not a leap year.";
                        if (Dmonth==2){
                            
                            if(Dhour==DAhour && Dmin==DAmin && Dsec<2){
                                        while(joy->downPressed()==0){spk->MarioTheme(); if(joy->downPressed()==1){break;}}
                                        Dsec=Dsec+2;
                                    }Dsec++;
                            if(Dsec==60){Dmin++;Dsec=0;}
                            if(Dmin==60){Dhour++;Dmin=0;spk->Chime();}
                            if(Dhour==24){Dday++;Dsec=0;Dmin=0;Dhour=0;}
                            if(Dday==29){Dmonth++;Dday=1;Dsec=0;Dmin=0;Dhour=0;}
                        }
                        if (Dmonth == 4 || Dmonth == 6 || Dmonth == 9 || Dmonth == 11){
                            
                            if(Dhour==DAhour && Dmin==DAmin && Dsec<2){
                                        while(joy->downPressed()==0){spk->MarioTheme(); if(joy->downPressed()==1){break;}}
                                        Dsec=Dsec+2;
                                    }Dsec++;
                            if(Dsec==60){Dmin++;Dsec=0;}
                            if(Dmin==60){Dhour++;Dmin=0;spk->Chime();}
                            if(Dhour==24){Dday++;Dsec=0;Dmin=0;Dhour=0;}
                            if(Dday==31){Dmonth++;Dday=1;Dsec=0;Dmin=0;Dhour=0;}
                        }
                        if (Dmonth == 1 || Dmonth == 3 || Dmonth == 5 || Dmonth == 7 || Dmonth == 9 || Dmonth ==12){
                            if(Dhour==DAhour && Dmin==DAmin && Dsec<2){
                                        while(joy->downPressed()==0){spk->MarioTheme(); if(joy->downPressed()==1){break;}}
                                        Dsec=Dsec+2;
                                    }Dsec++;
                            if(Dsec==60){Dmin++;Dsec=0;}
                            if(Dmin==60){Dhour++;Dmin=0;spk->Chime();}
                            if(Dhour==24){Dday++;Dsec=0;Dmin=0;Dhour=0;}
                            if(Dday==32){Dmonth++;Dday=1;Dsec=0;Dmin=0;Dhour=0;}
                        }
                        if (Dmonth == 13){Dyear++;Dmonth=1;Dday=1;Dsec=0;Dmin=0;Dhour=0;
                        }
                }
            }; 
            
            int getSecOnly(void){return SecOnly;};
                    
            int getHour(void){return Dhour;};
            int getMin (void){return Dmin;};
            int getSec (void){return Dsec;};
            int getDay (void){return Dday;};
            int getMonth (void){return Dmonth;};
            int getYear (void){return Dyear;};
            
            void SetTime (int hr, int mn){
                Dhour=hr;Dmin=mn;
            }
            
            void SetSeconds(int sec){
                Dsec=sec;
            }
            
            void SetYears(int year){
                Dyear=year;
            }
            
            void SetDayMonth(int day, int month){
                Dday=day;Dmonth=month;
            }
            void SetAlarm (int ahr, int amn, int asec){
                DAhour=ahr;DAmin=amn;DAsec=asec;
            }
            int getAlarmHour(void){return DAhour;};
            int getAlarmMin (void){return DAmin;};
            int getAlarmSec (void){return DAsec;}; 
};

class DisplayClock: public BasicClock {
    private:
        DisplayAll* disp;
        Ticker tick;
    public:
        DisplayClock (int dhour, int dmin, int dsec, int dday, int dmonth, int dyear, int seconly, Speaker* spk, Joystick* joy, DisplayAll* dis)
        : BasicClock(dhour, dmin, dsec, dday, dmonth, dyear, seconly,spk,joy), disp(dis){}
            
            void displayseconds(void){Counter();disp->PrintSecCounter(getSecOnly());}
            void displaynoseconds(void){Counter();}
            void showseconds (void){tick.attach(callback(this, & DisplayClock :: displayseconds), 1);} 
            void hideseconds (void) {disp->Clear();tick.attach(callback(this, & DisplayClock :: displaynoseconds), 1);}
            void displayactive(void){ Counter(); disp->Time (getHour(),getMin(),getSec(),getDay(),getMonth(),getYear());}
            void displaydeactive(void){Counter();}   
            void hidetime (void) {disp->Clear();tick.attach(callback(this, & DisplayClock :: displaydeactive), 1);}
            void showtime (void){tick.attach(callback(this, & DisplayClock :: displayactive), 1);}
};

class FSM{
    private: 
        _Screen Screen; 
        DisplayAll* disp;
        DisplayClock dclock;
        RotationHM rot;
        Speaker* spk;
     
    public:
        FSM (DisplayAll* dis, DisplayClock _dclock, RotationHM _rot, Speaker* Spk): disp(dis), dclock(_dclock), rot(_rot), spk(Spk){Screen=PrincipalScreen;};
             
        void WhenFire (bool f) {
              if (f) {  
                 switch(Screen){
                    case (PrincipalScreen): disp->Clear(); Screen = Menu;break;
                    
                    case (Menu): disp->Clear();Screen = PrincipalScreen;break;
                    
                    case (SetTimeScreen):dclock.SetTime(rot.getshour(), rot.getsmin()); rot.hideSaving();disp->Clear(); Screen = SetSeconds;break;
                    
                    case (SetYear):dclock.SetYears(rot.getsyear()); rot.hideSaving(); disp->Clear();Screen = SetMonthDay;break;
                    
                    case (SetMonthDay):dclock.SetDayMonth(rot.getsday(),rot.getsmonth());rot.hideSaving();disp->Clear(); Screen = Menu;break;
                                                           
                    case (SetAlarm):  dclock.SetAlarm(rot.getsAlarmhour(), rot.getsAlarmmin(),0);rot.hideSaving();disp->Clear(); Screen = Menu;break;
                    
                    case (DisplaySec): disp->Clear();Screen = Menu;break;
                    
                    case (PlayMario): disp->Clear();Screen= PrincipalScreen;break;
                    
                    case (SetSeconds): dclock.SetSeconds(rot.getssec());rot.hideSaving();disp->Clear();Screen= Menu;break;
                    
                }
            }
        };
        void WhenLeft (bool l){
            if(l){switch(Screen){                
                    case (Menu): disp->Clear(); Screen = SetTimeScreen;break;
                }
            }
        };
        void WhenRight (bool r) {
            if(r){switch(Screen){                
                    case (Menu): disp->Clear();  Screen = SetYear;break;
                }
            }
        };
        void WhenUp (bool u) {
            if(u){switch(Screen){                
                    case (Menu): disp->Clear(); Screen = SetAlarm;break;
                    case (PrincipalScreen): rot.hideSaving();dclock.hidetime();disp->Clear();disp->PrintMario();disp->PrintMario(); Screen = PlayMario;break;
                
                }
            }
        };
        void WhenDown (bool d) {
            if(d){switch(Screen){                
                    case (Menu): disp->Clear();disp->PrintSecCounter(0);; Screen = DisplaySec;break;
                }
            }
        }
        void states (void){
               switch(Screen){
                    case (PrincipalScreen): dclock.showtime();dclock.showtime();
                    /*spk->tickalarm();*/break;
                    case (Menu) : dclock.hideseconds ();dclock.hidetime(); disp->PrintMenu();break;
                    case (SetTimeScreen) : rot.updateRate();break;
                    case (SetAlarm) :  rot.updateRateAlarm();break;
                    case (DisplaySec) : dclock.showseconds();break;
                    case (PlayMario) :spk->BeepAlarm();break;
                    case (SetSeconds): rot.updateSeconds();break;
                    case (SetYear) : rot.updateYears();break;
                    case (SetMonthDay) : rot.updateMonthDay();break;
                }     
        }
        _Screen getstate (void){
            return Screen;
        }
};
               
int main() {
    
    C12832* Screen = new C12832(D11, D13, D12, D7, D10);
    Speaker* mySpeakers= new Speaker(D6);
    DisplayAll* D = new DisplayAll (Screen);
    Joystick* push= new Joystick(A2,A3,A4,A5,D4);
    DisplayClock cont (0,0,0,1,1,2018,0,mySpeakers,push, D);
    Potentiometer* leftHand = new Potentiometer(A0, 3.3);
    Potentiometer* rigthHand = new Potentiometer(A1, 3.3);
    RotationHM h(D, leftHand, rigthHand, 0.0, 3.3, 0, 24, 0, 60,0,60, 1, 32, 1, 13, 2018, 2058);// Maximum values for Min and Hours migth vary because of the range of the potentiometer 
    FSM fsm (D,cont,h,mySpeakers);
    
    fsm.states();
    
    while (true) {
    
        if (push->upPressed()|| push->downPressed() ||  push->leftPressed() || push->rightPressed() || push->firePressed()){
            fsm.WhenUp(push->upPressed());fsm.WhenDown(push->downPressed());fsm.WhenLeft(push->leftPressed()); 
            fsm.WhenRight(push->rightPressed());fsm.WhenFire(push->firePressed());fsm.states();
        }            
        if (fsm.getstate()==SetTimeScreen){leftHand->sample();rigthHand->sample();}
        if (fsm.getstate()==SetAlarm){leftHand->sample();rigthHand->sample();}
        if (fsm.getstate()==SetSeconds){rigthHand->sample();}
        if (fsm.getstate()==SetYear){ rigthHand->sample();}
        if (fsm.getstate()==SetMonthDay){leftHand->sample();rigthHand->sample();}
        wait(0.2);
    }
}
