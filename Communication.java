

import java.net.Socket;

/**
 * @brief Communication.
 *
 * @encoding UTF-8
 * @date 23 avr. 2015 at 08:48:56
 * @author rgv26
 */
public interface Communication {
    
    public void sendto_tcp();
    
    public void send_udp();
    
    public void receive_udp();
}
