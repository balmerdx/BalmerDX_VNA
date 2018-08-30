#include "float_to_string.h"

int floatToStringWithoutZero(char * outstr, int outstr_size, float value, int places, int minwidth, bool rightjustify)
{
    if(outstr_size<=0)
        return 0;
    // this is used to write a float value to string, outstr.  oustr is also the return value.
    int digit;
    float tens = 0.1;
    int tenscount = 0;
    int i;
    float tempfloat = value;
    int c = 0;
    int charcount = 1;
    int extra = 0;
    // make sure we round properly. this could use pow from <math.h>, but doesn't seem worth the import
    // if this rounding step isn't here, the value  54.321 prints as 54.3209

    // calculate rounding term d:   0.5/pow(10,places)  
    float d = 0.5;
    if (value < 0)
        d *= -1.0;
    // divide by ten for each decimal place
    for (i = 0; i < places; i++)
        d/= 10.0;    
    // this small addition, combined with truncation will round our values properly 
    tempfloat +=  d;

    // first get value tens to be the large power of ten less than value    
    if (value < 0)
        tempfloat *= -1.0;
    while ((tens * 10.0) <= tempfloat) {
        tens *= 10.0;
        tenscount += 1;
    }

    if (tenscount > 0)
        charcount += tenscount;
    else
        charcount += 1;

    if (value < 0)
        charcount += 1;
    charcount += 1 + places;

    minwidth += 1; // both count the null final character
    if (minwidth > charcount){        
        extra = minwidth - charcount;
        charcount = minwidth;
    }

    if (extra > 0 && rightjustify)
    {
        for (int i = 0; i< extra; i++)
        {
            outstr[c++] = NUM_SPACE;
            if(c>=outstr_size)
                return c;
        }
    }

    // write out the negative if needed
    if (value < 0)
    {
        outstr[c++] = '-';
        if(c>=outstr_size)
            return c;
    }

    if (tenscount == 0)
    {
        outstr[c++] = '0';
        if(c>=outstr_size)
            return c;
    }

    for (i=0; i< tenscount; i++)
    {
        digit = (int) (tempfloat/tens);
        outstr[c++] = digit+'0';
        if(c>=outstr_size)
            return c;
        tempfloat = tempfloat - ((float)digit * tens);
        tens /= 10.0;
    }

    // if no places after decimal, stop now and return

    // otherwise, write the point and continue on
    if (places > 0)
    {
        outstr[c++] = '.';
        if(c>=outstr_size)
            return c;
    }


    // now write out each decimal place by shifting digits one by one into the ones place and writing the truncated value
    for (i = 0; i < places; i++)
    {
        tempfloat *= 10.0; 
        digit = (int) tempfloat;
        outstr[c++] = digit + '0';
        if(c>=outstr_size)
            return c;
        // once written, subtract off that digit
        tempfloat = tempfloat - (float) digit; 
    }

    if (extra > 0 && !rightjustify)
    {
        for (int i = 0; i<extra; i++)
        {
            outstr[c++] = NUM_SPACE;
            if(c>=outstr_size)
                return c;
        }
    }


    return c;
}


int floatToString(char * outstr, int outstr_size, float value, int places, int minwidth, bool rightjustify)
{
    int chars = floatToStringWithoutZero(outstr, outstr_size-1, value, places, minwidth, rightjustify);
    outstr[chars] = 0;
    return chars;
}

void intToString(char st[27], long num, int length, char filler)
{
    char buf[25];
    bool neg=false;
    int c=0, f=0;

    if (num==0)
    {
        if (length!=0)
        {
            for (c=0; c<(length-1); c++)
                st[c]=filler;
            st[c]=48;
            st[c+1]=0;
        }
        else
        {
            st[0]=48;
            st[1]=0;
        }
    }
    else
    {
        if (num<0)
        {
            neg=true;
            num=-num;
        }

        while (num>0)
        {
            buf[c]=48+(num % 10);
            c++;
            num=(num-(num % 10))/10;
        }
        buf[c]=0;

        if (neg)
        {
            st[0]=45;
        }

        if (length>(c+neg))
        {
            for (int i=0; i<(length-c-neg); i++)
            {
                st[i+neg]=filler;
                f++;
            }
        }

        for (int i=0; i<c; i++)
        {
            st[i+neg+f]=buf[c-i-1];
        }
        st[c+neg+f]=0;

    }

}
