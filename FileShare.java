import java.io.BufferedReader;
import java.io.File;
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
		
		char [] cbuf = new char[6];
		char [] buf = new char[140]; 
		BufferedReader fr = new BufferedReader(new FileReader(new File(nomFichier)));
		
		
		w.write(cmd.toCharArray());
		w.flush();
		
		r.read(cbuf);
		
		System.out.println("Recu :  "+ new String(cbuf));
		
		if(new String(cbuf,0,6).equals("GIVE\r\n")){
			
			System.out.println("Envoi fichier " +nomFichier);
			
			while(fr.read(buf) != -1){
				
				cmd = "DATA " + new String(buf) + CR + LF;
				w.write(cmd.toCharArray());
				w.flush();
			}
			
			cmd = "ENDF" + CR + LF;
			w.write(cmd.toCharArray());
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
	
	
	public static void reception(Socket socket,BufferedReader r, PrintWriter w, String nomFichier){
		
		
	}
	
	
	
	
	
	
	
}














