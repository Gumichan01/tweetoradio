import java.io.*;
import java.net.*;
import java.lang.*;

public class Abonne{

	public static void main(String[] args){

		try{

			MulticastSocket mso=new MulticastSocket(4094);
			mso.joinGroup(InetAddress.getByName("228.1.2.3"));

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

