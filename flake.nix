{
	description = "B compiler environment";

	inputs.nixpkgs.url = "github:NixOS/nixpkgs";

	outputs = { self, nixpkgs }:
	{
		devShells =
		{
			x86_64-linux = let
				pkgs = import nixpkgs { system = "x86_64-linux"; };
			in
			{
				default = pkgs.mkShell
				{
					packages =
					[
						pkgs.qemu
						pkgs.coreutils
						pkgs.flex
						pkgs.bison
					];

					shellHook = ''
						echo "let's B."
						'';
				};
			};
		};
	};
}
