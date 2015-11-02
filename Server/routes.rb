resource "/players" do
  get "/", to: "PlayerController#index"
  get "/connect/white", to: "PlayerController#connect1"
  get "/connect/black", to: "PlayerController#connect2"
end

resource "/map" do
  get "/", to: "MapControler#index"
end

resource "/game" do
  get "/", to: "GameController#get_round"
  get "/status", to: "GameController#status"
  post "/play/:x/:y", to: "GameController#play_round"
end
