namespace ChaoticLib{
	namespace Direct2D{
		bool Initialize();
		void Uninitialize();
	}

	bool Initialize()
	{
		return Direct2D::Initialize();
	}

	void Uninitialize()
	{
		Direct2D::Uninitialize();
	}

}
