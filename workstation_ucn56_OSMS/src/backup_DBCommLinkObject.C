narseTagID(rcvBuffer, rcvlen);   

    return true;
}
void DBCommLinkObject::parseTagID(unsigned char *buf, int buflen)
{
    int nHeaderLen = 2+2+2+2; // 2 : TT, 2 : Total Idx, 2 : Start Idx, 2 : End Idx   
    int nBufLenCount = nHeaderLen;

    char * portNo;
    int i = 0;
    int fdes;

    extern DBCommLinkObject     DBCommLink; 

    // Common Analog & Digital
    unsigned char picassoId[4];
    unsigned char pid[12], desc[36], sig_type[4], pipbitmap[4];
    unsigned char tagID[12] = {0,};

    // Analog Only
    unsigned char disfmt[8], eng_units[16], met_units[16], erange_lo[4], erange_hi[4], mfactor[4];
    unsigned char moffset[4], hhval[4], hval[4], lval[4], llval[4], hhmet[4], hmet[4], lmet[4]; 
    unsigned char llmet[4], lIPN[2], hIPN[2]; 
    
    // Digital Only 
    unsigned char message1[12], message2[12]; 
    
    // Common Pisa
    unsigned char pisabitmap[4]; 
    char newsta[4], value[4];
    
    char szTemp[2];
    
    char szPort[MAX_PORTNO_LEN] = {0,};
    char szDBFileName[20] = {0,};
    char szRcvTagID[20] = {0,}; 
    portNo = DBCommLink.getportNo();

    if( !strcmp( portNo, "3333" ))       sprintf(szDBFileName, "%s", "dbimage_3.bin" );
    else if( !strcmp( portNo, "4444" ))  sprintf(szDBFileName, "%s", "dbimage_4.bin" );
    else if( !strcmp( portNo, "5555" ))  sprintf(szDBFileName, "%s", "dbimage_5.bin" );
    else if( !strcmp( portNo, "6666" ))  sprintf(szDBFileName, "%s", "dbimage_6.bin" );
    else                                 sprintf(szDBFileName, "%s", "dbimage_3.bin" );

    sprintf(szRcvTagID, "%s", "RCV_TAGID.txt");

    char szTotIdx[5] = {0,};
    char szCurStartIdx[5] = {0,};
    char szCurEndIdx[5] = {0,};

    sprintf((char*)szTotIdx, "%02x%02x", buf[2], buf[3]);
    sprintf((char*)szCurStartIdx, "%02x%02x", buf[4], buf[5]);
    sprintf((char*)szCurEndIdx, "%02x%02x", buf[6], buf[7]);

    int nTotalIdx = 0;
    int nCurStartIdx = 0;
    int nCurEndIdx = 0;
    int nSigType = 0;

    sscanf(szTotIdx, "%x", &nTotalIdx);
    sscanf(szCurStartIdx, "%x", &nCurStartIdx);
    sscanf(szCurEndIdx, "%x", &nCurEndIdx);

    if(strncmp((char*)buf, "TT", 2) == 0 ) 
    {
        if( nCurStartIdx == 1 )	
        {
            if( -1 == unlink( szDBFileName )) // Fail to delete DB File 
            {
               if( ENOENT != errno )
	       {
                   cout << "Error Deleting " << szDBFileName << ". errno = " << errno << endl;
	           cout << " Recommand manually deleting " << szDBFileName << endl;
	       }
	       else 
	           cout << " " << szDBFileName << " does not exist." << endl;
            }
	    else cout << szDBFileName << " DB is successfully deleted." << endl;

            if( -1 == unlink( szRcvTagID )) // Fail to delete RCV_TAGID.txt file
	    {
                if( ENOENT != errno )
		{
                   cout << "Error Deleting " << szDBFileName << ". errno = " << errno << endl;
	           cout << " Recommand manually deleting " << szRcvTagID << endl;
		}
		else
	           cout << " " << szRcvTagID << " does not exist." << endl;
	    }
	    else cout << szRcvTagID << " File is successfully deleted." << endl;

	    eventFile.open(szDBFileName, ios::binary | ios::app );
            
	    tagIDFile.open(szRcvTagID, ios::app);
            chmod("RCV_TAGID", 0777);

	    // Retry DB File Open
            if( !eventFile.is_open()) eventFile.open(szDBFileName, ios::binary | ios::app );

	    // Retry RCV_TAGID.txt file open
	    if( !tagIDFile.is_open()) tagIDFile.open(szRcvTagID);
        }

	for( i=nCurStartIdx; i<=nCurEndIdx; i++ ) // 8 PID per 1 Packet. But it may be different.... 
        {
            // PICASSO ID
	    memset(picassoId, 0x00, sizeof(picassoId));
            eventFile.write((char*)picassoId, sizeof(picassoId));

	    // PID
	    if( ((buf+nBufLenCount) != NULL) && ((buf+nBufLenCount) != '\0')) 
	    { 
	        memset(pid, 0x00, sizeof(pid));
		strncpy((char*)pid, (char*)(buf + nBufLenCount), 8); // PID
	    }	
	    else memset(pid, 0x00, sizeof(pid));
	    
	    eventFile.write((char*)pid, sizeof(pid)); 
	    nBufLenCount += 8;

	    // SIG TYPE
            if( ((buf+nBufLenCount) != NULL) && ((buf+nBufLenCount) != '\0')) 
            {
                //strncpy((char*)sig_type, (char*)(buf + nBufLenCount), 2); // SIG_TYPE
                //sprintf((char*)sig_type, "%x%x", buf[nBufLenCount], buf[nBufLenCount+1]);
                sig_type[0] = 0x00;
		sig_type[1] = 0x00;
		sig_type[2] = buf[nBufLenCount];
		sig_type[3] = buf[nBufLenCount+1];
		
		sscanf((char*)sig_type, "%x", &nSigType);
	    
	        if(nSigType >= 10) tagIDFile << pid << ":" << "D" << endl;      
	        else               tagIDFile << pid << ":" << "A" << endl;
	    }
            else if((buf+nBufLenCount) == 0x00)
	    {
		memset(sig_type, 0x00, sizeof(sig_type));
	    }
	    nBufLenCount += 2;

	    // DESC
	    if( ((buf+nBufLenCount) != NULL) && ((buf+nBufLenCount) != '\0')) 
            {
                strncpy((char*)desc, (char*)buf + nBufLenCount, 36); // DESCRIPTION
            }
	    else if( (buf+nBufLenCount) == 0x00)
	    {
		memset(desc, 0x00, sizeof(desc)); 
	    }
	    eventFile.write((char*)desc, sizeof(desc)); 
	    nBufLenCount += 36;
           
            // TAGID
            memset(tagID, 0x00, sizeof(tagID));
	    eventFile.write((char*)tagID, sizeof(tagID));

	    // PIP BITMAP
	    if( ((buf+nBufLenCount) != NULL) && ((buf+nBufLenCount) != '\0')) 
            { 
	        strncpy((char*)pipbitmap, (char*)(buf + nBufLenCount), 4); // PIPBITMAP
            }
            else if( (buf+nBufLenCount) == 0x00)
	    {
		memset(pipbitmap, 0x00, sizeof(pipbitmap));
	    }
	    eventFile.write((char*)pipbitmap, sizeof(pipbitmap));
	    nBufLenCount += 4;

	    eventFile.write((char*)sig_type, sizeof(sig_type)); 

	    // DISPLAY FORMAT    
	    if( ((buf+nBufLenCount) != NULL) && ((buf+nBufLenCount) != '\0')) 
	    {
	        char szParse[8] = {0,};
	        strncpy((char*)szParse, (char*)(buf + nBufLenCount), 8);

	        int nDisFmt = atoi(szParse);
	        if(nDisFmt == 0) sprintf((char*)disfmt, "%s", "%6.0f");
	        else if(nDisFmt == 1) sprintf((char*)disfmt, "%s", "%6.1f");
       	        else if(nDisFmt == 2) sprintf((char*)disfmt, "%s", "%6.2f");
	        else if(nDisFmt == 3) sprintf((char*)disfmt, "%s", "%6.3f");
	        else if(nDisFmt == 4) sprintf((char*)disfmt, "%s", "%6.4f");
	        else if(nDisFmt == 5) sprintf((char*)disfmt, "%s", "%8.1e");
	        else sprintf((char*)disfmt, "%s", "?????"); 
	    }
	    else if( (buf+nBufLenCount) == 0x00)
	    {
	        memset(disfmt, 0x00, sizeof(disfmt));
	    }
	    eventFile.write((char*)disfmt, sizeof(disfmt));
	    nBufLenCount += 8;
            
	    // ENGLISH UNITS
	    if( ((buf+nBufLenCount) != NULL) && ((buf+nBufLenCount) != '\0')) 
	    {
	        strncpy((char*)eng_units, (char*)buf + nBufLenCount, 16);
	    }
	    else if( (buf+nBufLenCount) == 0x00)
	    {  
	        memset(eng_units, 0x00, sizeof(eng_units));
            }	
	    eventFile.write((char*)eng_units, sizeof(eng_units));
	    nBufLenCount += 16;

	    // METRIC UNITS
            if( ((buf+nBufLenCount) != NULL) && ((buf+nBufLenCount) != '\0')) 
            { 
	        strncpy((char*)met_units, (char*)(buf + nBufLenCount), 16);
	    }
	    else if( (buf+nBufLenCount) == 0x00)
	    {
	        memset(met_units, 0x00, sizeof(met_units));
	    }
	    eventFile.write((char*)met_units, sizeof(met_units));
	    nBufLenCount += 16;

	    // ENGINEERING RANGE LO LIMIT
            if( ((buf+nBufLenCount) != NULL) && ((buf+nBufLenCount) != '\0')) 
	    {
	        strncpy((char*)erange_lo, (char*)(buf + nBufLenCount), 4);
	    }
	    else if((buf+nBufLenCount) == 0x00)
	    {
	        memset(erange_lo, 0x00, sizeof(erange_lo));
            }
	    eventFile.write((char*)erange_lo, sizeof(erange_lo));
	    nBufLenCount += 4;
            
	    // ENGINEERING RANGE HI LIMIT
	    if( ((buf+nBufLenCount) != NULL) && ((buf+nBufLenCount) != '\0')) 
	    {
	        strncpy((char*)erange_hi, (char*)(buf + nBufLenCount), 4);
	    }
	    else if( (buf+nBufLenCount) == 0x00) 
	    {
	        memset(erange_hi, 0x00, sizeof(erange_hi)); 
            }	
	    eventFile.write((char*)erange_hi, sizeof(erange_hi));
	    nBufLenCount += 4;
	    
	    // M-FACTOR ( METRIC -> ENGLISH CONVERSION FACTOR )
	    if( ((buf+nBufLenCount) != NULL) && ((buf+nBufLenCount) != '\0')) 
	    {
	        strncpy((char*)mfactor, (char*)(buf + nBufLenCount), 4);
	    }
	    else if( (buf+nBufLenCount) == 0x00)
	    {
	        memset(mfactor, 0x00, sizeof(mfactor));
            }	
   	    eventFile.write((char*)mfactor, sizeof(mfactor));
            nBufLenCount += 4;
            
	    // M_OFFSET ( METRIC -> ENGLISH CONVERSION OFFSET ) 
	    if( ((buf+nBufLenCount) != NULL) && ((buf+nBufLenCount) != '\0')) 
	    { 
	        strncpy((char*)moffset, (char*)(buf + nBufLenCount), 4);
	    } 
	    else if( (buf+nBufLenCount) == 0x00)
	    {
	        memset(moffset, 0x00, sizeof(moffset));
            }
	    eventFile.write((char*)moffset, sizeof(moffset));
	    nBufLenCount += 4;
            
	    // HI HI LIMIT ENGLISH VALUE
	    if( ((buf+nBufLenCount) != NULL) && ((buf+nBufLenCount) != '\0')) 
	    { 
	        strncpy((char*)hhval, (char*)(buf + nBufLenCount), 4);
	    }
	    else if( (buf+nBufLenCount) == 0x00)
	    {
	        memset(hhval, 0x00, sizeof(hhval));
	    }
	    eventFile.write((char*)hhval, sizeof(hhval));
	    nBufLenCount += 4;
            
	    // HI LIMIT ENGLISH VALUE 
	    if( ((buf+nBufLenCount) != NULL) && ((buf+nBufLenCount) != '\0')) 
	    {
	        strncpy((char*)hval, (char*)(buf + nBufLenCount), 4);
	    }
	    else if( (buf+nBufLenCount) == 0x00) 
	    {
	        memset(hval, 0x00, sizeof(hval));
            }	
	    eventFile.write((char*)hval, sizeof(hval));
	    nBufLenCount += 4;
            
	    // LO LIMIT ENGLISH VALUE 
	    if( ((buf+nBufLenCount) != NULL) && ((buf+nBufLenCount) != '\0')) 
	    {
	        strncpy((char*)lval, (char*)(buf + nBufLenCount), 4);
	    }
	    else if( (buf+nBufLenCount) == 0x00)
	    {
	        memset(lval, 0x00, sizeof(lval));
	    }
	    eventFile.write((char*)lval, sizeof(lval));
	    nBufLenCount += 4;
           
	    // LO LO LIMIT METRIC VALUE 
	    if( ((buf+nBufLenCount) != NULL) && ((buf+nBufLenCount) != '\0')) 
	    {
	        strncpy((char*)llval, (char*)(buf + nBufLenCount), 4);
	    }
	    else if((buf+nBufLenCount) == 0x00)
	    {
	        memset(llval, 0x00, sizeof(llval));
            }
	    eventFile.write((char*)llval, sizeof(llval));
	    nBufLenCount += 4;
           
	    // HI HI LIMIT METRIC VALUE
	    if( ((buf+nBufLenCount) != NULL) && ((buf+nBufLenCount) != '\0')) 
	    {
	        strncpy((char*)hhmet, (char*)(buf + nBufLenCount), 4);
	    }
	    else if((buf+nBufLenCount) == 0x00)
	    {
	        memset(hhmet, 0x00, sizeof(hhmet));
            }	
	    eventFile.write((char*)hhmet, sizeof(hhmet));
	    nBufLenCount += 4;
            
	    // HI LIMIT METRIC VALUE
	    if( ((buf+nBufLenCount) != NULL) && ((buf+nBufLenCount) != '\0')) 
	    {
	        strncpy((char*)hmet, (char*)(buf + nBufLenCount), 4);
	    }
	    else if((buf+nBufLenCount) == 0x00)
	    {
	        memset(hmet, 0x00, sizeof(hmet));
            }
	    eventFile.write((char*)hmet, sizeof(hmet));
	    nBufLenCount += 4;
            
	    // LO LIMIT METRIC VALUE 
	    if( ((buf+nBufLenCount) != NULL) && ((buf+nBufLenCount) != '\0')) 
	    {
	        strncpy((char*)lmet, (char*)(buf + nBufLenCount), 4);
	    }
	    else if((buf+nBufLenCount) == 0x00)
	    {
	        memset(lmet, 0x00, sizeof(lmet));
            }	
	    eventFile.write((char*)lmet, sizeof(lmet));
	    nBufLenCount += 4;
            
	    // LO LO LIMIT METRIC VALUE
	    if( ((buf+nBufLenCount) != NULL) && ((buf+nBufLenCount) != '\0')) 
	    {
	        strncpy((char*)llmet, (char*)(buf + nBufLenCount), 4);
	    } 
	    else if((buf+nBufLenCount) == 0x00)
	    {
	        memset(llmet, 0x00, sizeof(llmet));
            }	
	    eventFile.write((char*)llmet, sizeof(llmet));
	    nBufLenCount += 4;
            
	    // L-IPN ( NON-ZERO = IPN OF VARIABLE SETPOINT )
	    if( ((buf+nBufLenCount) != NULL) && ((buf+nBufLenCount) != '\0')) 
	    { 
	        strncpy((char*)lIPN, (char*)(buf + nBufLenCount), 2);
            }	
	    else if( (buf+nBufLenCount) == 0x00)
	    {
	        memset(lIPN, 0x00, sizeof(lIPN));
	    }
	    eventFile.write((char*)lIPN, sizeof(lIPN));
	    nBufLenCount += 2;
            
	    // H-IPN ( NON-ZERO = IPN OF VARIABLE SETPOINT )
	    if( ((buf+nBufLenCount) != NULL) && ((buf+nBufLenCount) != '\0')) 
	    {
	        strncpy((char*)hIPN, (char*)(buf + nBufLenCount), 2);
            }
	    else if( (buf+nBufLenCount) == 0x00)
	    {
                memset(hIPN, 0x00, sizeof(hIPN));
            }
	    eventFile.write((char*)hIPN, sizeof(hIPN));
	    nBufLenCount += 2;
	  
	    // MESSAGE 1
            if( ((buf+nBufLenCount) != NULL) && ((buf+nBufLenCount) != '\0')) 
	    {
                strncpy((char*)message1, (char*)(buf + nBufLenCount), 8); // message1
	    }
	    else if( (buf+nBufLenCount) == 0x00)
	    {
                memset(message1, 0x00, sizeof(message1)); 
	    }
	    eventFile.write((char*)message1, sizeof(message1));
	    nBufLenCount += 8;

	    // MESSAGE 2
            if( ((buf+nBufLenCount) != NULL) && ((buf+nBufLenCount) != '\0')) 
            {
                strncpy((char*)message2, (char*)(buf + nBufLenCount), 8); // message2
            }
            else if( (buf+nBufLenCount) == 0x00)
            {	
                memset(message2, 0x00, sizeof(message2));
            }
	    eventFile.write((char*)message2, sizeof(message2));
            nBufLenCount += 8;
            
	    // PISA BITMAP
	    if( ((buf+nBufLenCount) != NULL) && ((buf+nBufLenCount) != '\0')) 
	    {
	        strncpy((char*)pisabitmap, (char*)(buf + nBufLenCount), 4);
	    } 
	    else if( (buf+nBufLenCount) == 0x00)
	    {
	        memset(pisabitmap, 0x00, sizeof(pisabitmap));
	    }
	    eventFile.write((char*)pisabitmap, sizeof(pisabitmap));
	    nBufLenCount += 4;
	
	    // Do not receive STATUS & VALUE packet. But must fit DB file format 
	    memset(newsta, 0x00, sizeof(newsta));
	    eventFile.write(newsta, sizeof(newsta));
	   
	    memset(value, 0x00, sizeof(value));
	    eventFile.write(value, sizeof(value));
            
	    // Check whether if 2Bytes of End is EE or NOT.
	    // If Yes, close DB image file.
	    if( (nCurEndIdx == nTotalIdx) && (buf[nBufLenCount] == 0x45) && (buf[nBufLenCount+1] == 0x45)) 
            {
                eventFile.close();
		cout << "DB( " << szDBFileName << " ) is successfully closed." << endl;
		
		tagIDFile.close();
		cout << szRcvTagID << " File is successfully closed." << endl;
	    }// end of loop
	} // end of packet-1
    }
    return;
}
// end of parseTagID

void swap32(char * pval)
{
    char tmp;
    tmp = *(pval+3);
    *(pval+3) = *(pval);
    *(pval) = tmp;
    tmp = *(pval+2);
    *(pval+2) = *(pval+1);
    *(pval+1) = tmp;
}

