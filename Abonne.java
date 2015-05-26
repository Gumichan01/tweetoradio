import java.io.*;
import java.net.*;
import java.lang.*;

public class Abonne{

	public static void main(String[] args){

		int i;

		try{

			MulticastSocket mso=new MulticastSocket(new Integer(args[1]).intValue());
			mso.joinGroup(InetAddress.getByName(args[0]));

			byte[]data=new byte[161];
			DatagramPacket paquet=new DatagramPacket(data,data.length);

			while(true){
				mso.receive(paquet);

				String st=new String(paquet.getData());

				i = st.length()-3;

				while(i > 0 && st.charAt(i) == '#'){
					i--;
				}
				
				System.out.println(st.substring(0,i+1));
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

