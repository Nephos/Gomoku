resource "/players" do
  get "/connect", to: "PlayerController#connect"
end

resource "/game" do
  get "/", to: "GameController#request_round"
  get "/wait", to: "GameController#request_round"
  get "/map", to: "GameController#request_map"
  post "/play/:x/:y", to: "GameController#play_round"
  get "/play/:x/:y", to: "GameController#play_round" # DEBUG
  resource "/options" do
    get "/break5/:set", to: "GameController#break5"
    get "/free3/:set", to: "GameController#free3"
  end
end
