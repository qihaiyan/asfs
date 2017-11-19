import java.util.*;
import java.io.*;
import java.net.*;

public class ftr {
    public static void main(String[] args) {
      try{
        Socket s = new Socket("98.35.11.15",6101);
        System.out.println("connect ok");
        try{
        	ftrutil ftrutil = new ftrutil();
        	ftrutil.SndFile(s,"ftrutil.class","aa.class");
        	ftrutil.RcvFile(s,"RIDE.rar","RIDE.rar");
        	ftrutil.RcvFile(s,"/home/ap/fileserv/report/350000000/20080701/RIDE.rar","RIDE.rar");
        	ftrutil.RcvFile(s,"bb.txt","bb.txt");
        	ftrutil.RcvFile(s,"bb.txt","bb.txt");
        	ftrutil.DirList(s,"/home/ap/fileserv/report/350000000/20080701/RIDE.rar");
        	ftrutil.DirList(s,"/home/ap/fileserv/report/350000000/20080701/RIDE.rar");
        	ftrutil.DirList(s,"/home/ap/fileserv/report/350000000/20080701/RIDE.rar");
        	ftrutil.DirList(s,"/home/ap/fileserv/report/350000000/20080701/RIDE.rar");
        	ftrutil.DirList(s,"/home/ap/fileserv/report/350000000/20080701/RIDE.rar");
        	ftrutil.DirList(s,"/home/ap/fileserv/report/350000000/20080701/RIDE.rar");
        	ftrutil.DirList(s,"/home/ap/fileserv/report/350000000/20080701/RIDE.rar");
        	ftrutil.DirList(s,"/home/ap/fileserv/report/350000000/20080701/RIDE.rar");
        	ftrutil.DirList(s,"/home/ap/fileserv/report/350000000/20080701/RIDE.rar");
        	ftrutil.DirList(s,"/home/ap/fileserv/report/350000000/20080701/RIDE.rar");
        	ftrutil.DirList(s,"/home/ap/fileserv/report/350000000/20080701/RIDE.rar");
        	ftrutil.DirList(s,"/home/ap/fileserv/report/350000000/20080701/RIDE.rar");
        	ftrutil.DirList(s,"/home/ap/fileserv/report/350000000/20080701/RIDE.rar");
        	ftrutil.DirList(s,"/home/ap/fileserv/report/350000000/20080701/RIDE.rar");
        	ftrutil.DirList(s,"/home/ap/fileserv/report/350000000/20080701/RIDE.rar");
        	ftrutil.RcvFile(s,"RIDE.rar","RIDE.rar");
        	ftrutil.DirList(s,"/home/ap/fileserv/report/350000000/20080701/RIDE.rar");
        	ftrutil.DirList(s,"/home/ap/fileserv/report/350000000/20080701/RIDE.rar");
        	ftrutil.DirList(s,"/home/ap/fileserv/report/350000000/20080701/RIDE.rar");
        	ftrutil.DirList(s,"/home/ap/fileserv/report/350000000/20080701/RIDE.rar");
        	ftrutil.RcvFile(s,"bb.txt","bb.txt");
        	ftrutil.SndFile(s,"ftrutil.class","aa.class");
        }
        finally{
	      s.close();
        }
      }
      catch (IOException e){
        e.printStackTrace();
      }
    }
}