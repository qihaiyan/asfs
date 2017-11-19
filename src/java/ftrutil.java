/**
 *
 * Created: Tue Mar 21 22:48:30 2006
 *
 * @version 1.0
 */
import java.util.*;
import java.lang.*;
import java.io.*;
import java.net.*;

public class ftrutil {

    final public static byte FPUTREQ  = 112; /* 'p' */
    final public static byte FGETREQ  = 103; /* 'g' */
    final public static byte FLISTREQ = 108; /* 'i' */
    final public static byte FRESP    = 97 ; /* 'a' */
    final public static byte FFBREQ   = 49 ; /* '1' */
    final public static byte FFBRESP  = 50 ; /* '2' */

    final public static byte FTR_SNDEND  = 48 ; /* '0' */
    final public static byte FTR_SNDOK   = 49 ; /* '1' */
    final public static byte FTR_SNDERR  = 50 ; /* '2' */

    final public static byte FTR_RCVEND  = 48 ; /* '0' */
    final public static byte FTR_RCVOK   = 49 ; /* '1' */
    final public static byte FTR_RCVERR  = 50 ; /* '2' */

    final public static byte FTR_SERVEROK = 48; /* '0' */
    final public static byte FTR_SDISABLE = 49; /* '1' */
    final public static byte FTR_SFOPERR  = 50; /* '2' */

    final public static int STD_DGSIZE   = 5;
    final public static int FNAMELEN     = 256;
    final public static int FTR_FSNDBUF  = 1024;

    final public static int DG_HEAD_LEN  = 1+STD_DGSIZE;

    private class ftr_dg {

        byte dgtype;             /* Datagram type                        */
        Integer  dglength;       /* Datagram length,which is decoded into a byte array[STD_DGSIZE] */
        byte barray[];

        public ftr_dg() {
/**/        	barray=new byte[2048];
/**///        	dglength=DG_HEAD_LEN;
/**/			setLen(DG_HEAD_LEN);
        }

        public void setLen(Integer len) {
        	dglength=len;
/**/			String s = String.format("%d",dglength);
			byte[] bb=dglength.toString().getBytes();
			for (int i=0;i<STD_DGSIZE;i++){
				if (i<bb.length) barray[i+1]=bb[i];
				else barray[i+1]=0;
/**/			}
        }
    
        public Integer getLen() {
        	return dglength;
        }

        public byte[] getByteArray() {
        	return barray;
        }

        public byte[] toByteArray() {

        	barray[0]=dgtype;

//        	System.arraycopy(bb,0,barray,1,Math.min(bb.length,STD_DGSIZE));

        	return barray;
        }

        public void fromByteArray(byte[] ba) {
        	dgtype=ba[0];
        	byte bb[] = new byte[STD_DGSIZE];
        	System.arraycopy(ba,1,bb,0,STD_DGSIZE);
        	String s = new String(bb);
        	dglength = Integer.parseInt(s.trim());
        }

        public void fromByteArray() {
        	fromByteArray(barray);
        }
    }

    private class ftr_req extends ftr_dg {

        String sfile;       /* Server file name,which is decoded into a byte array[FNAMELEN]  */

        public ftr_req() {
        	this.setLen(DG_HEAD_LEN+FNAMELEN);
        }

        public byte[] toByteArray() {
            super.toByteArray();
        	System.arraycopy(sfile.getBytes(),0,barray,DG_HEAD_LEN,Math.min(sfile.getBytes().length,FNAMELEN));
        	return barray ;
        }
    }

    private class ftr_resp extends ftr_dg {

        byte respstat;                 /* Response status                      */

        public ftr_resp() {
        	dgtype=FRESP;
        	this.setLen(DG_HEAD_LEN+1);
        }

        public byte[] toByteArray() {

            super.toByteArray();
            barray[DG_HEAD_LEN] = respstat ;
        	return barray ;
        }

        public void fromByteArray() {
        	respstat = barray[DG_HEAD_LEN] ;
        }
    }

    /*
    **   FTR Data Sending Request Datagram
    */
    private class ftr_fsndreq extends ftr_dg {

        byte sndstat;                             /* Sending status                       */
        byte databuf[] = new byte[FTR_FSNDBUF];   /* Sending data area                    */

        public ftr_fsndreq() {
        	dgtype=FFBREQ;
        	sndstat=FTR_SNDOK;
        }

        public byte[] toByteArray() {

            super.toByteArray();

        	barray[DG_HEAD_LEN] = sndstat;
        	System.arraycopy(databuf,0,barray,DG_HEAD_LEN+1,dglength-DG_HEAD_LEN-1);
        	return barray ;
        }

        public void fromByteArray() {
        	super.fromByteArray();
        	sndstat = barray[DG_HEAD_LEN] ;
        	System.arraycopy(barray,DG_HEAD_LEN+1,databuf,0,dglength-DG_HEAD_LEN-1);
        }
    }

    /*
    **   FTR Data Sending Response Datagram
    */
    private class ftr_frcvresp extends ftr_dg {

        byte rcvstat;               /* Receiving status   */

        public ftr_frcvresp() {
        	dgtype=FFBRESP;
        	this.setLen(DG_HEAD_LEN+1);
        }

        public byte[] toByteArray() {
            super.toByteArray();
        	barray[DG_HEAD_LEN] = rcvstat ;
        	return barray ;
        }

        public void fromByteArray() {
        	rcvstat = barray[DG_HEAD_LEN] ;
        }
    };

    /*
    **   FTR File Sending Function : SndFile
    */
    public int SndFile(Socket conn,String localfile,String remotefile) 
    {
        ftr_req ftr_req_o = new ftr_req();
        ftr_resp ftr_resp_o = new ftr_resp();
        ftr_fsndreq ftr_fsndreq_o = new ftr_fsndreq();
        ftr_frcvresp ftr_frcvresp_o = new ftr_frcvresp();

        try {
            FileInputStream fin = new FileInputStream(localfile);
            
            InputStream DGin = conn.getInputStream();
            OutputStream DGout = conn.getOutputStream();

            /* Send Request Datagram FPUTREQ */
            ftr_req_o.dgtype=FPUTREQ;
            ftr_req_o.sfile=localfile;
            DGout.write(ftr_req_o.toByteArray(),0,ftr_req_o.getLen());

            /* Receive Response Datagram from Server of FPUTREQ */
            DGin.read(ftr_resp_o.toByteArray(),0,ftr_resp_o.getLen());
            ftr_resp_o.fromByteArray();
            if (ftr_resp_o.respstat != FTR_SERVEROK) {
            	return -1;
            }

            /* Read Block of Local File and Send to Server */
            int count=0;
            ftr_fsndreq_o.sndstat = FTR_SNDOK;
            while ((count = fin.read(ftr_fsndreq_o.databuf)) > 0) {
            	ftr_fsndreq_o.setLen(DG_HEAD_LEN+1+count);
            	DGout.write(ftr_fsndreq_o.toByteArray(),0,ftr_fsndreq_o.getLen());
//            	System.out.printf("count=%d,SND STAT=%d\n",count,ftr_fsndreq_o.sndstat);
            }

            /* Notify the Server that File has been completely sent */
            ftr_fsndreq_o.sndstat = FTR_SNDEND;
      	    ftr_fsndreq_o.setLen(DG_HEAD_LEN+1);
       	    DGout.write(ftr_fsndreq_o.toByteArray(),0,ftr_fsndreq_o.getLen());
//           	System.out.printf("count=%d,SND STAT=%d\n",count,ftr_fsndreq_o.sndstat);

            /* Receive Status from Server */
            DGin.read(ftr_frcvresp_o.toByteArray(),0,ftr_frcvresp_o.getLen());
            ftr_frcvresp_o.fromByteArray();
            if (ftr_frcvresp_o.rcvstat == FTR_RCVOK) {
            	System.out.println("发送文件成功");
            }
            else {
            	System.out.printf("发送文件失败,Send Status=%d,RETCODE=%d",ftr_fsndreq_o.sndstat,ftr_frcvresp_o.rcvstat);
            	return -1;
            }
        }
        catch (IOException e){
            e.printStackTrace();
        }
        return 0;
    }

    /*
    **   FTR File Receiving Function : RcvFile
    */
    public int RcvFile(Socket conn,String remotefile,String localfile)
    {
       	ftr_dg ftr_dg_o = new ftr_dg();
        ftr_req ftr_req_o = new ftr_req();
        ftr_resp ftr_resp_o = new ftr_resp();
        ftr_fsndreq ftr_fsndreq_o = new ftr_fsndreq();
        ftr_frcvresp ftr_frcvresp_o = new ftr_frcvresp();

        try {
            FileOutputStream fout = new FileOutputStream(localfile);
            
            InputStream DGin = conn.getInputStream();
            OutputStream DGout = conn.getOutputStream();

            /* Send Request Datagram FGETREQ */
            ftr_req_o.dgtype=FGETREQ;
            ftr_req_o.sfile=remotefile;
            DGout.write(ftr_req_o.toByteArray(),0,ftr_req_o.getLen());

            /* Receive Response Datagram from Server of FGETREQ */
            DGin.read(ftr_resp_o.toByteArray(),0,ftr_resp_o.getLen());
            ftr_resp_o.fromByteArray();
            if (ftr_resp_o.respstat != FTR_SERVEROK) {
            	System.out.println("服务器不在正常状态");
            	return -1;
            }

            /* Read block from Server and write to a Local File */
            while (true) {
            	/* Read the Datagram Head first , and Get the Datagram Length */
            	int len,pos;
            	len=DG_HEAD_LEN;
            	pos=0;
				while(len>0){
	            	int a=DGin.read(ftr_dg_o.getByteArray(),pos,len);
	            	len-=a;
	            	pos+=a;
	            }
            	ftr_dg_o.fromByteArray();
            	ftr_fsndreq_o.setLen(ftr_dg_o.getLen());

            	/* Read the left part of Datagram, which contains the File Block */
				len=ftr_fsndreq_o.getLen()-DG_HEAD_LEN;
				pos=DG_HEAD_LEN;
				while(len>0){
	            	int a=DGin.read(ftr_fsndreq_o.getByteArray(),pos,len);
	            	len-=a;
	            	pos+=a;
				}
            	ftr_fsndreq_o.fromByteArray();

            	/* Write the block to Local File */
            	if (ftr_fsndreq_o.sndstat == FTR_SNDEND) {
            		ftr_frcvresp_o.rcvstat=FTR_RCVOK;
            		DGout.write(ftr_frcvresp_o.toByteArray(),0,ftr_frcvresp_o.getLen());
            		System.out.println("文件接收成功");
            		return 0;
            	}
            	else if (ftr_fsndreq_o.sndstat == FTR_SNDOK) {
            		fout.write(ftr_fsndreq_o.databuf,0,ftr_fsndreq_o.getLen()-DG_HEAD_LEN-1);
            	}
            	else if (ftr_fsndreq_o.sndstat == FTR_SNDERR) {
            		System.out.println("Send Status Error");
            		return -1;
            	}
            	else {
            		System.out.println("incorrect Send Status code");
            		return -1;
            	}
            }
        }
        catch (IOException e){
            e.printStackTrace();
        }
        return 0;
    }

    /*
    **   FTR Dir List Function : DirList
    */
    public int DirList(Socket conn,String dirpath)
    {
       	ftr_dg ftr_dg_o = new ftr_dg();
        ftr_req ftr_req_o = new ftr_req();
        ftr_resp ftr_resp_o = new ftr_resp();
        ftr_fsndreq ftr_fsndreq_o = new ftr_fsndreq();
        ftr_frcvresp ftr_frcvresp_o = new ftr_frcvresp();

        try {
            FileOutputStream fout = new FileOutputStream("aa.txt");
            
            InputStream DGin = conn.getInputStream();
            OutputStream DGout = conn.getOutputStream();

            /* Send Request Datagram FGETREQ */
            ftr_req_o.dgtype=FLISTREQ;
            ftr_req_o.sfile=dirpath;
            DGout.write(ftr_req_o.toByteArray(),0,ftr_req_o.getLen());

            /* Receive Response Datagram from Server of FGETREQ */
            DGin.read(ftr_resp_o.toByteArray(),0,ftr_resp_o.getLen());
            ftr_resp_o.fromByteArray();
            if (ftr_resp_o.respstat != FTR_SERVEROK) {
            	System.out.println("服务器不在正常状态");
            	return -1;
            }

            /* Read Dir List from Server and write to a List */
            while (true) {
            	/* Read the Datagram Head first , and Get the Datagram Length */
            	int len,pos;
            	len=DG_HEAD_LEN;
            	pos=0;
				while(len>0){
	            	int a=DGin.read(ftr_dg_o.getByteArray(),pos,len);
	            	len-=a;
	            	pos+=a;
	            }
            	ftr_dg_o.fromByteArray();
            	ftr_fsndreq_o.setLen(ftr_dg_o.getLen());

            	/* Read the left part of Datagram, which contains the File Info */
				len=ftr_fsndreq_o.getLen()-DG_HEAD_LEN;
				pos=DG_HEAD_LEN;
				while(len>0){
	            	int a=DGin.read(ftr_fsndreq_o.getByteArray(),pos,len);
	            	len-=a;
	            	pos+=a;
				}
            	ftr_fsndreq_o.fromByteArray();

            	/* Write the block to Local List */
            	if (ftr_fsndreq_o.sndstat == FTR_SNDEND) {
            		ftr_frcvresp_o.rcvstat=FTR_RCVOK;
            		DGout.write(ftr_frcvresp_o.toByteArray(),0,ftr_frcvresp_o.getLen());
            		System.out.println("文件列表接收成功");
            		return 0;
            	}
            	else if (ftr_fsndreq_o.sndstat == FTR_SNDOK) {
            		byte[] bb;
            		bb = new byte[ftr_fsndreq_o.getLen()-DG_HEAD_LEN-1];
            		System.arraycopy(ftr_fsndreq_o.databuf,0,bb,0,ftr_fsndreq_o.getLen()-DG_HEAD_LEN-1);
            		System.out.printf("%s\n",new String(bb));
            	}
            	else if (ftr_fsndreq_o.sndstat == FTR_SNDERR) {
            		System.out.println("Send Status Error");
            		return -1;
            	}
            	else {
            		System.out.println("Send Status Error");
            		return -1;
            	}
            }
        }
        catch (IOException e){
            e.printStackTrace();
        }
        return 0;
    }
}
