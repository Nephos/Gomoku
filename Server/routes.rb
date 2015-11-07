resource "/players" do
  get "/connect", to: "PlayerController#connect"
end

resource "/game" do
  #resource "/:id" do
    get "/", to: "GameController#request_round"
    post "/play/:x/:y", to: "GameController#play_round"
    get "/play/:x/:y", to: "GameController#play_round" # DEBUG
  #end
end
