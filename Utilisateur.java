

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.UnknownHostException;
import java.util.Scanner;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 * @brief Utilisateur.
 *
 * @encoding UTF-8
 * @date 27 mars 2015 at 21:35:21
 * @author rgv26 : Pierre Dibo, Universite Diderot Paris 7 - L3 Informatique
 * @email rgv26.warforce@hotmail.fr
 */
public class Utilisateur {

    static Client client = null;
    static Scanner sc = new Scanner(System.in);

    /**
     * @param args
     */
    public static void main(String[] args) {
        String id, ip = "", tmp = "";
        int len, menu, port;

        String test = "ACKM\r\n";
        /*
         if (args.length != 2) {
         System.err.println("usage : " + args[0] + " <ID>\n");
         System.exit(1);
         }

         id = args[1];
         */
        id = "Salut";

        if (id.length() > 8) {
            System.err.println("id " + id + " du client non conforme.");
            System.exit(1);
        } else {
            if (id.length() < 8) {
                len = id.length() - 8;
                while (len-- > 0) {
                    tmp += "#";
                }
                tmp += id;
                id = tmp;
            }
        }

        if (id.equals("########")) {
            id = null;
        }

        client = new Client(id);

        try {
            System.out.println("le client " + id + " d'ip " + InetAddress.getLocalHost().getHostAddress());
        } catch (UnknownHostException ex) {
            Logger.getLogger(Utilisateur.class.getName()).log(Level.SEVERE, null, ex);
        }

        while (true) {
            menu();
            menu = sc.nextInt();
            switch (menu) {
                case 0:
                    System.exit(0);
                    break;
                case 1:
                    client.sendto_tcp();
                    break;
                case 2: //TODO
                    client.receive_udp();
                    break;
                case 3: // TODO
                    break;
                default:
                    System.out.println(menu + " n'est pas pris en compte dans le menu.\nRetour au menu.\n\n");
                    break;
            }
        }

    }

    /* Diplay menu */
    private static void menu() {
        System.out.println("Menu utilisateur.\n"
                + "1 : Entrez une commande.\n"
                + "2 : Ecoutez un diffuseur.\n"
                + "0 : Arret du client.\n");
    }

}
