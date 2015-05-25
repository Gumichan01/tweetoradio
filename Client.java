

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.MulticastSocket;
import java.net.Socket;
import java.net.SocketException;
import java.util.HashMap;
import java.util.Map;
import java.util.Scanner;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 * @brief Client.
 *
 * @encoding UTF-8
 * @date 27 mars 2015 at 21:36:25
 * @author rgv26
 * @email rgv26.warforce@hotmail.fr
 */
public class Client implements Communication {

    public final HashMap<String, Item> listDiff = new HashMap<String, Item>();
    private final HashMap<String, Tweet> listTweet = new HashMap<String, Tweet>();
    private final String identifiant;
    private static int compteur = 1;
    PrintWriter pw;
    BufferedReader br;

    public String getIdentifiant() {
        return identifiant;
    }

    /**
     * @fn public Client()
     *
     * @brief Default constructor of Client
     */
    public Client() {
        identifiant = "#######" + compteur++;
    }

    /**
     * @fn public Client(String id)
     *
     * @brief Constructor of Client
     *
     * @param id
     */
    public Client(String id) {
        if (id == null) {
            identifiant = "#######" + compteur++;
        } else {
            identifiant = id;
        }
    }

    public void displayListDiffuseurs() {
        for (Map.Entry<String, Item> entry : listDiff.entrySet()) {
            System.out.println("ITEM " + entry.getKey() + " " + entry.getValue().toString());
        }
    }

    public void displayListTweets() {
        for (Map.Entry<String, Tweet> entry : listTweet.entrySet()) {
            System.out.println("OLDM " + entry.getKey() + entry.getValue().toString());
        }
    }

    private String int_to_string_nb_mess(int n) {
        if (n >= 1000) {
            return null;
        } else {
            if (n > 99 && n < 1000) {
                return String.valueOf(n);
            } else {
                if (n > 9 && n < 100) {
                    return ("0" + String.valueOf(n));
                } else {
                    if (n >= 0) {
                        return ("00" + String.valueOf(n));
                    } else {
                        return null;
                    }
                }
            }
        }
    }

    private void get_listDiff(String type) throws IOException {
        String msgSend, msgRcv;
        String id = new String(), ip_multicast = new String(), port_multicast = new String(),
                ip_machine = new String(), port_machine = new String();
        Integer num_diff;
        int index = 5;

        msgSend = type + "\r\n";
        pw.print(msgSend);
        pw.flush();
        msgRcv = br.readLine();

        if (msgRcv != null && msgRcv.substring(0, 4).equals("LINB") ) {
            System.out.println(msgRcv + "\n");
            num_diff = new Integer(msgRcv.substring(msgRcv.length() - 2, msgRcv.length()));
            if (num_diff < 0 || num_diff > 99) {
                System.err.println("num_diff à recevoir incorrect - sendto()");
            } else {
                for (int i = 0; i < num_diff; i++) {
                    msgRcv = br.readLine();
                    if (msgRcv.substring(0, 4).equals("ITEM")) {

                        while ((int) msgRcv.charAt(index) != 32) {
                            id += msgRcv.charAt(index++);
                        }

                        while ((int) msgRcv.charAt(++index) != 32) {
                            ip_multicast += msgRcv.charAt(index);
                        }

                        while ((int) msgRcv.charAt(++index) != 32) {
                            port_multicast += msgRcv.charAt(index);
                        }

                        while ((int) msgRcv.charAt(++index) != 32) {
                            ip_machine += msgRcv.charAt(index);
                        }

                        while (++index < msgRcv.length()) {
                            port_machine += msgRcv.charAt(index);
                        }
                        listDiff.put(id, new Item(id, ip_multicast, port_multicast, ip_machine, port_machine));
                    } else {
                        System.err.println("type reçu incorrect - sendto()");
                        break;
                    }
                }
            }
        }
    }

    private void get_listTweets(String type, Scanner sc) throws IOException {
        String msgSend, msgRcv, nb_mess, id, num_mess, tweet;

        System.out.println("Ecrire le nombre de message que vous voulez lire : \n");
        nb_mess = int_to_string_nb_mess(sc.nextInt());
        if (nb_mess == null) {
            System.err.println("nb_mess à voir incorrect - sendto()");
            pw.close();
            br.close();
        }
        msgSend = type + " " + nb_mess + "\r\n";
        pw.print(msgSend);
        pw.flush();

        for (int i = 0; i < Integer.parseInt(nb_mess); i++) {
            msgRcv = br.readLine();
            if (msgRcv != null) {
                if (msgRcv.substring(0, 4).equals("OLDM")) {
                    num_mess = msgRcv.substring(5, 9);
                    id = msgRcv.substring(9, 18);
                    tweet = msgRcv.substring(19, msgRcv.length());
                    listTweet.put(num_mess, new Tweet(id, num_mess, tweet));
                } else {
                    if (msgRcv.substring(0, 4).equals("ENDM")) {
                        System.out.println("ENDM");
                        return;
                    } else {
                        System.err.println("type reçu incorrect, attendu : ENDM - sendto()");
                        return;
                    }
                }
            } else {
                System.err.println("erreur - sendto()");
                return;
            }
        }

        if (br.readLine().substring(0, 4).equals("ENDM")) {
            System.out.println("ENDM");
        } else {
            System.err.println("type reçu incorrect, attendu : ENDM - sendto().\n");
        }
    }

    private void send_mess(String type) throws IOException {
        String msgSend = "", msgRcv, tweet;
		Scanner sc = new Scanner(System.in);

        System.out.println("Ecrire le message que vous voulez envoyer : \n");
        tweet = sc.next();
        while (tweet.length() > 140) {
            System.out.println("Message trop grand. Recommencez : \n");
            tweet = sc.next();
        }
        msgSend += type + " " + identifiant + " " + tweet + "\r\n";
        pw.print(msgSend);
        pw.flush();
        msgRcv = br.readLine();

        if (msgRcv.substring(0, 4).equals("ACKM")) {
            System.out.println(msgRcv);
        } else {
            System.err.println("message reçu incorrect, attendu : ACKM - sendto().\n");
        }
    }

    @Override
    public void sendto_tcp() {
        Socket sock = null;
        InetAddress ia = null;
        Scanner sc = new Scanner(System.in);
        String ip, type_msg;
        int port;

        try {
            System.out.println("Entrez une ip : ");
            ip = sc.next();
            ia = InetAddress.getByName(ip);
            System.out.println("Entrez un port : ");
            port = sc.nextInt();
            sock = new Socket(ia, port);
            pw = new PrintWriter(new OutputStreamWriter(sock.getOutputStream()));
            br = new BufferedReader(new InputStreamReader(sock.getInputStream()));
            System.out.println("Ecrire le type message à envoyer : \n");
            type_msg = sc.next();

            if (type_msg.substring(0, 4).equals("LIST")) {
                this.get_listDiff(type_msg);
                this.displayListDiffuseurs();
            }

            if (type_msg.substring(0, 4).equals("LAST")) {
                this.get_listTweets(type_msg, sc);
                if (!listTweet.isEmpty()) {
                    this.displayListTweets();
                } else {
                    System.out.println("Le diffuseur ne contient aucun message.\n");
                }
            }

            if (type_msg.substring(0, 4).equals("MESS")) {
                this.send_mess(type_msg);
            }

            pw.close();
            br.close();
        } catch (IOException ex) {
            System.err.println(ex.getMessage() + "\nsendto()\n");
        }
    }

    @Override
    public void send_udp() {
        Scanner sc = new Scanner(System.in);
        String msgSend = "", msgRcv, ip, type_msg, tweet;
        byte[] dataSend, dataRcv = new byte["ACKM\r\n".getBytes().length];
        int port;
        try {
            DatagramSocket dso = new DatagramSocket();
            System.out.println("Entrez l'adresse ip multicast : ");
            ip = sc.next();
            System.out.println("Entrez son port : ");
            port = sc.nextInt();
            InetSocketAddress ia = new InetSocketAddress(ip, port);
            System.out.println("Ecrire le type message à envoyer : \n");
            type_msg = sc.next();

            if (type_msg.substring(0, 4).equals("MESS")) {
                System.out.println("Ecrire le message que vous voulez envoyer : \n");
                tweet = sc.next();
                while (tweet.length() > 140) {
                    System.out.println("Message trop grand. Recommencez : \n");
                    tweet = sc.next();
                }
                msgSend += type_msg + " " + identifiant + " " + tweet + "\r\n";
                dataSend = msgSend.getBytes();
                DatagramPacket paquet = new DatagramPacket(dataSend, dataSend.length, port);
                dso.send(paquet);
                paquet = new DatagramPacket(dataRcv, dataRcv.length, port);
                dso.receive(paquet);
                msgRcv = new String(paquet.getData(), 0, paquet.getLength());
                if (msgRcv.substring(0, msgRcv.length()).equals("ACKM\r\n")) {
                    System.out.println("Réception du tweet par le diffuseur.\n");
                    dso.close();
                } else {
                    System.err.println("type reçu incorrect, attendu : ENDM - sendto().\n");
                    dso.close();
                }
            }

        } catch (IOException ex) {
            System.err.println("erreur - send_udp().\n");
        }
    }

    @Override
    public void receive_udp() {
        Scanner sc = new Scanner(System.in);
        String msgSend = "", msgRcv, ip, type_msg, tweet;
        byte[] dataRcv = new byte[Constante.DIFF_LENGTH];
        int port;
        System.out.println("Entrez l'adresse ip d'écoute multicast : ");
        ip = sc.next();
        System.out.println("Entrez son port : ");
        port = sc.nextInt();
        try {
           	MulticastSocket mso = new MulticastSocket(port);
           	mso.joinGroup(InetAddress.getByName(ip));
            DatagramPacket paquet = new DatagramPacket(dataRcv, dataRcv.length);
            while (true) {
                mso.receive(paquet);
                msgRcv = new String(paquet.getData());
                System.out.println(msgRcv);
            }
        } catch (IOException ex) {
            ex.printStackTrace();
        }
    }
}
