import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.InetAddress;
import java.net.Socket;





class FileShare{
	
	public final static String CR = "\r";
	public final static String LF = "\n";
	
	public static void main(String [] args){
		
		String opt;
		String ip;
		String nomFichier;
		int port;
		
		
		try{
			
			if(args.length < 4){
				

				System.err.println("java  [-s|-g] <@IP> <port> <nomFicher>");
				System.err.println("-s : envoyer un fichier \n-g : recevoir un fichier");
				return;
			}
			
			opt = args[0];
			ip = args[1];
			port = new Integer(args[2]).intValue();
			nomFichier = args[3];
			
			Socket socket = new Socket(InetAddress.getByName(ip),port);
			
			BufferedReader r = new BufferedReader(new InputStreamReader(socket.getInputStream()));
			PrintWriter w = new PrintWriter(socket.getOutputStream());
			
			
			if(opt.equals("-s")){
				
				envoi(socket, r, w, nomFichier);
				
			}else if(opt.equals("-g")){
				reception(socket, r, w, nomFichier);
			}else{
				
				System.err.println("Option invalide");
				System.err.println("java  [-s|-g] <@IP> <port> <nomFicher>");
				System.err.println("-s : envoyer un fichier \n-g : recevoir un fichier");
			}
			
			socket.close();
			
		
		}catch(NumberFormatException ne){
			
			ne.printStackTrace();
		
		}catch(SecurityException se){
			
			se.printStackTrace();
			
		}catch(IOException ioe){
			
			ioe.printStackTrace();
		}
		
		
	}
	

	public static void envoi(Socket socket,BufferedReader r, PrintWriter w, String nomFichier) throws IOException{
		
		String cmd = "SETF " + nomFichier + CR + LF;
		String s;
		
		char [] cbuf = new char[6];
		byte [] buf = new byte[140]; 
		FileInputStream fr = new FileInputStream(new File(nomFichier));
		
		
		w.write(cmd.toCharArray());
		w.flush();
		
		r.read(cbuf);
		
		System.out.println("Recu :  "+ new String(cbuf));
		
		int n;
		
		if(new String(cbuf,0,6).equals("GIVE\r\n")){
			
			System.out.println("Envoi fichier " +nomFichier);
			
			try{
				
				while((n = fr.read(buf)) != -1){
					
					s = new String(buf,0,n);
					cmd = "DATA " + s + CR + LF;
					//System.out.println(s + "\n OKDOKI GO");
					System.out.println("DATA " + s + CR + LF);
					
					w.write("DATA " + s + CR + LF);
					w.flush();
					buf = new byte[140];
					
					try{
						// On temporise pour laisser le temps au diffuseur 
						// de récupérer les informations
						Thread.sleep(1000);
					}catch(Exception e){	
						e.printStackTrace();
					}
					
				}
				
			}finally{
				
				fr.close();
			}
			
			
			// Fin dumessage
			w.write("ENDF" + CR + LF);
			w.flush();
			
			r.read(cbuf);
			
			System.out.println("Recu :  "+ new String(cbuf));
			
			if(new String(cbuf,0,6).equals("ACKM\r\n"))
				System.out.println("OK fait ");
			else
				System.out.println("KO problème");
		
		}else{
			
			System.out.println("Problème avec GIVE - " + new String(cbuf));
		}
		
	}
	
	
	public static void reception(Socket socket,BufferedReader r, PrintWriter w, String nomFichier) throws IOException{
		

		String s = null;
		
		int n;
		char [] cbuf = new char[140 + 5 +2];
		byte [] buf = new byte[140 + 5 +2]; 
		FileOutputStream fr = new FileOutputStream(new File(nomFichier));	
		
		
		try{
			
			w.write("GETF " + nomFichier + CR + LF);
			w.flush();

			
			//System.out.println(s);
			
			while((n = r.read(cbuf)) != -1
					&& new String(cbuf,0,n).subSequence(0, 5).equals("DATA ")){
				
				s = new String(cbuf,0,n);

				System.out.println("Recu  "+n + " octets depuis le diffuseur");
				
				
				fr.write(s.substring(5, n).getBytes());
				fr.flush();
				
			}
			
			System.out.println("Fin de reception");
			
			
		}finally{
			
			fr.close();
		}
		
		
		
	}
	
	
	
	
	
	
	
}














