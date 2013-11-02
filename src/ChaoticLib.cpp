namespace ChaoticLib{
	namespace Win32{
		bool Initialize();
		void Uninitialize();
	}	
	namespace Direct2D{
		bool Initialize();
		void Uninitialize();
	}

	bool Initialize()
	{
		return Win32::Initialize() && Direct2D::Initialize();
	}

	void Uninitialize()
	{
		Win32::Uninitialize();
		Direct2D::Uninitialize();
	}

}
