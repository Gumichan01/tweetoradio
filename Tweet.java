/**
 * @brief Tweet.
 *
 * @encoding UTF-8
 * @date 23 mai 2015 at 17:16:48
 * @author rgv26
 * @email rgv26.warforce@hotmail.fr
 */
public class Tweet {

    public final String id, num_mess, message;

    /**
     * @fn public Tweet()
     *
     * @brief Default constructor of Tweet
     *
     * @param p_id
     * @param p_num_mess
     * @param p_msg
     */
    public Tweet(String p_id, String p_num_mess, String p_msg) {
        id = p_id;
        num_mess = p_num_mess;
        message = p_msg;
    }

    @Override
    public String toString() {
        for (int i = message.length() - 1; i >= 0; i--) {
            if (message.charAt(i) != '#') {
                return id + " " + message.substring(0, i + 1) + "\n";
            }
        }

        return id + " " + message + "\n";
    }
}
