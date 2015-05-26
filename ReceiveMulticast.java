import java.io.*;
import java.net.*;
import java.lang.*;

public class ReceiveMulticast {

	public static void main(String[] args){
	
		if(args.length < 2){
		
			System.out.println("java ReceiveMulticast <@IP> <port>");
			return;
		}
	
	
		try{
		
			MulticastSocket mso=new MulticastSocket(new Integer(args[1]).intValue());
			mso.joinGroup(InetAddress.getByName(args[0]));

			byte[]data=new byte[161];
			DatagramPacket paquet=new DatagramPacket(data,data.length);
			
			while(true){
				mso.receive(paquet);
				
				String st=new String(paquet.getData());
				System.out.println(st);
			}
		} catch(NumberFormatException ne){
			ne.printStackTrace();
		}
		catch(IOException ioe){
			ioe.printStackTrace();
		}
		catch(SecurityException se){
			se.printStackTrace();
		}
	}
}

