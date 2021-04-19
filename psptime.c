//#include "psptime.h"

char* convertTOchar(int val)
    { 
    	int ones, tens, hundreds, thousands;
	if(val > 999){
		static char str[5];
		ones = val % 10;
		tens = (val%100)/10;
		hundreds = (val%1000)/100;
		thousands = val/1000;
    	str[0] =  thousands + 48;
    	str[1] =  hundreds + 48;
    	str[2] =  tens + 48;
    	str[3] =  ones + 48;
    	str[4] =  '\0';
	return str;
	}
		else if(val > 99 && val <= 999){
    		static char str[4];
	    	ones = val % 10;
	     	tens = (val%100)/10;
            hundreds = val/100;
	    	str[0] =  hundreds + 48;
    		str[1] =  tens + 48;
    		str[2] =  ones + 48;
    		str[3] =  '\0';
	return str;
	}
		else{
    		static char str[3];
	    	ones = val % 10;
	     	tens = val/10;
    		str[0] =  tens + 48;
    		str[1] =  ones + 48;
    		str[2] =  '\0';
	return str;
	}

}

char* getYear(){
      		int tempy = sceKernelLibcTime((void *) 0);
			int year = (tempy / 31536000) + 1970;
			static char *yr;
			yr = convertTOchar(year);
            return yr;
            }

char* getMonth(){
			int tempy = sceKernelLibcTime((void *) 0);
			int days_this_year = (tempy - 1104537600)/(24*60*60);
            static char *month;
			if(days_this_year <= 31){month = "January";}
			else if(days_this_year > 31 && days_this_year <= 59){month = "Febuary";}
			else if(days_this_year > 59 && days_this_year <= 90){month = "March";}
			else if(days_this_year > 90 && days_this_year <= 120){month = "April";}
			else if(days_this_year > 120 && days_this_year <= 151){month = "May";}
			else if(days_this_year > 151 && days_this_year <= 181){month = "June";}
			else if(days_this_year > 181 && days_this_year <= 212){month = "July";}
			else if(days_this_year > 212 && days_this_year <= 243){month = "August";}
			else if(days_this_year > 243 && days_this_year <= 273){month = "September";}
			else if(days_this_year > 273 && days_this_year <= 304){month = "October";}
			else if(days_this_year > 304 && days_this_year <= 334){month = "November";}
			else if(days_this_year > 334 && days_this_year <= 365){month = "December";}
	return month;
}

char* getDay(){
            int tempy = sceKernelLibcTime((void *) 0);
   			int days_this_year = (tempy - 1104537600)/(24*60*60);
            int day_of_month;
            static char *day;
      
       		if(days_this_year <= 31){day_of_month = 31 - days_this_year;}
			else if(days_this_year > 31 && days_this_year <= 59){day_of_month = 59 - days_this_year;}
			else if(days_this_year > 59 && days_this_year <= 90){day_of_month = 90 - days_this_year;}
			else if(days_this_year > 90 && days_this_year <= 120){day_of_month = 120 - days_this_year;}
			else if(days_this_year > 120 && days_this_year <= 151){day_of_month = 151 - days_this_year;}
			else if(days_this_year > 151 && days_this_year <= 181){day_of_month = 181 - days_this_year;}
			else if(days_this_year > 181 && days_this_year <= 212){day_of_month = 212 - days_this_year;}
			else if(days_this_year > 212 && days_this_year <= 243){day_of_month = 243 - days_this_year;}
			else if(days_this_year > 243 && days_this_year <= 273){day_of_month = 273 - days_this_year;}
			else if(days_this_year > 273 && days_this_year <= 304){day_of_month = 304 - days_this_year;}
			else if(days_this_year > 304 && days_this_year <= 334){day_of_month = 334 - days_this_year;}
			else if(days_this_year > 334 && days_this_year <= 365){day_of_month = 365 - days_this_year;}
      
            day = convertTOchar(day_of_month);
            return day;
}

