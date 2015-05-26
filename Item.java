/**
 * @brief Item.
 *
 * @encoding UTF-8
 * @date 22 mai 2015 at 20:59:00
 * @author rgv26
 * @email rgv26.warforce@hotmail.fr
 */
public class Item {

    public String id, ip_multicast, port_multicast, ip_machine, port_machine;

    /**
     * @fn public Item()
     *
     * @brief Default constructor of Item
     *
     * @param p_id
     * @param p_ip_multicast
     * @param p_port_multicast
     * @param p_ip_machine
     * @param p_port_machine
     */
    public Item(String p_id, String p_ip_multicast, String p_port_multicast, String p_ip_machine, String p_port_machine) {
        id = p_id;
        ip_multicast = p_ip_multicast;
        port_multicast = p_port_multicast;
        ip_machine = p_ip_machine;
        port_machine = p_port_machine;
    }

    @Override
    public String toString() {
        String s = "";
        s += ip_multicast + " " + port_multicast
                + " " + ip_machine + " " + port_machine;
        return s;
    }

}
