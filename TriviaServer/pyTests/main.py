import Client

typicalData: dict = {
    'password': "PassOne2!@",
    'email': "idoengel0707@gamil.com"
}


@Client.try_again(try_for=8)
def init_client() -> Client.Client:
    """
    create new client with new data, init the pipe of the sock
    :return: new client instance
    """
    if not hasattr(init_client, "counter"):
        init_client.counter = 1
        init_client.port = 55555

    curr_username: str = f"TempCli{init_client.counter}"
    cli = Client.Client(init_client.port, curr_username, **typicalData)
    print(f"new client: {cli}", end="\n\n\n")

    init_client.port += 1
    init_client.counter += 1

    return cli


def main() -> None:
    """
    main entry-point
    """
    cli1: Client.Client = init_client()
    cli1.login()

    cli1.create_room("RM", num_of_questions=2)
    cli1.start_game()


if __name__ == "__main__":
    main()
