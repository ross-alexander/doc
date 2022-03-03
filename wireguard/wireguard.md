    umask 077
    wg genkey | tee privatekey | wg pubkey > publickey

$include /etc/wireguard/wg0.conf

    wg-quick up wg0

