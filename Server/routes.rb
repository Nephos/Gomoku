resource "/players" do
  get "/connect", to: "PlayerController#connect"
end

resource "/game" do
  get "/", to: "GameController#request_round"
  get "/debug", to: "GameController#debug"
  get "/map", to: "GameController#request_map"
  post "/play/:x/:y", to: "GameController#play_round"
  get "/play/:x/:y", to: "GameController#play_round" # DEBUG
end
