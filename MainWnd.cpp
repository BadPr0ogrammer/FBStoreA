#include "fbxsdk.h"
#include "FbxLoader.h"
#include "App.h"
#include "MainWnd.h"
#include "LoginDlg.h"
#include "FBase.h"
#include "Utilities.h"

using namespace System::Windows::Data;
using namespace System::Windows::Markup;
using namespace System::IO;
using namespace System::Text;
using namespace System::Diagnostics;
using namespace System::Numerics;
using namespace Microsoft::Win32;

using namespace HelixToolkit;
using namespace HelixToolkit::Geometry;
using namespace HelixToolkit::Wpf;

using namespace fbxsdk;
using namespace std;
using uchar = unsigned char;

namespace FBStoreA
{
	MainWnd::MainWnd()
	{
		FileStream^ fs = gcnew FileStream("MainWnd.xaml", FileMode::Open);
		_root = (DependencyObject^)XamlReader::Load(fs);
		fs->Close();

		Window^ tmp = (Window^)_root;
		WindowStartupLocation = tmp->WindowStartupLocation;
		Content = tmp->Content;
		DataContext = this;

		MenuItem^ file_mi = (MenuItem^)tmp->FindName("MenuItem_File");
		file_mi->FontSize = 20;

		MenuItem^ open_mi = (MenuItem^)tmp->FindName("MenuItem_Open");
		open_mi->Click += gcnew RoutedEventHandler(this, &MainWnd::OnOpenFile);

		_login_mi = (MenuItem^)tmp->FindName("MenuItem_Login");
		_login_mi->FontSize = 20;
		_login_mi->Click += gcnew RoutedEventHandler(this, &MainWnd::OnLogin);

		_user_mi = (MenuItem^)tmp->FindName("MenuItem_User");
		_user_mi->FontSize = 20;
		_user_mi->Click += gcnew RoutedEventHandler(this, &MainWnd::OnUser);

		BindMenuItem(_login_mi, "PropLogin");
		BindMenuItem(_user_mi, "PropUser");

		CommandBinding^ closeBinding = gcnew CommandBinding(ApplicationCommands::Close);
		closeBinding->Executed += gcnew ExecutedRoutedEventHandler(this, &MainWnd::OnClose);
		this->CommandBindings->Add(closeBinding);

		_modelGroup = gcnew Model3DGroup();
		_modelVisual = gcnew ModelVisual3D();

		HelixViewport3D^ viewport = (HelixViewport3D^)tmp->FindName("Viewport");
		viewport->Children->Add(_modelVisual);
		_modelVisual->Content = _modelGroup;
	}

	void MainWnd::OnLogin(Object^ sender, RoutedEventArgs^ e)
	{
		App^ app = (App^)Application::Current;
		if (app->_fbptr->IsLogin())
		{
			app->_fbptr->Logout();
			PropLogin = "Login";
			PropUser = "";
		}
		else
		{
			LoginDlg^ loginDlg = gcnew LoginDlg();
			auto result = loginDlg->ShowDialog();
			if (result)
			{
				if (loginDlg->_anon)
				{
					if (app->_fbptr->LoginAnon())
					{
						PropLogin = "Logout";
						PropUser = "<Anon.>";
						MessageBox::Show("Login Anonymously!", "FBStoreA");
					}
				} else if (loginDlg->_tokenEndpoint != nullptr) {
					std::string access_token = ConvertToStdString(loginDlg->_tokenEndpoint["access_token"]);
					std::string id_token = ConvertToStdString(loginDlg->_tokenEndpoint["id_token"]);
					if (app->_fbptr->Login(id_token, access_token))
					{
						PropLogin = "Logout";
						PropUser = gcnew String(app->_fbptr->_user.c_str());
						std::string msg = "Login " + app->_fbptr->_user + " !";
						MessageBox::Show(gcnew String(msg.c_str()), "FBStoreA");
					}
				}
			}
		}
	}

	void MainWnd::OnClose(Object^ sender, ExecutedRoutedEventArgs^ e) 
	{
		App^ app = (App^)Application::Current;
		delete app->_fbptr;
		this->Close(); 

		delete _fbxLoader;
		_fbxLoader = nullptr;

		/////////////_modelVisual->Clear()
		_modelGroup = nullptr;

	}

	void MainWnd::OnPropertyChanged(String^ propertyName)
	{
		PropertyChanged(this, gcnew PropertyChangedEventArgs(propertyName));
	}

	void MainWnd::BindMenuItem(MenuItem^ menuItem, String^ propertyName)
	{
		Binding^ binding = gcnew Binding(propertyName);
		binding->Source = this;
		menuItem->SetBinding(MenuItem::HeaderProperty, binding);
	}

	void MainWnd::BuildMeshes(FbxNode* node)
	{
		if (!node)
			return;
		int geometryCount = node->GetSrcObjectCount<FbxGeometry>();
		for (int i = 0; i < geometryCount; i++)
		{
			FbxGeometry* geometry = node->GetSrcObject<FbxGeometry>(i);
			if (geometry && geometry->GetAttributeType() == FbxNodeAttribute::eMesh)
			{
				auto greenMt = MaterialHelper::CreateMaterial(Colors::Green);

				FbxMesh* mesh = geometry->GetNode()->GetMesh();
				auto meshBuilder = gcnew MeshBuilder(false, false, false);

				FbxLayer* layer = mesh->GetLayer(0);
				FbxLayerElementMaterial* pMaterialLayer = layer->GetMaterials();
				int numMatIndices = pMaterialLayer->GetIndexArray().GetCount();
				for (int k = 0; k < numMatIndices; k++)
				{
					int matIndex = pMaterialLayer->GetIndexArray()[k];
					FbxSurfaceMaterial* smat = mesh->GetNode()->GetMaterial(matIndex);
					if (smat->GetClassId().Is(FbxSurfaceLambert::ClassId))
					{
						FbxSurfaceLambert* lam = (FbxSurfaceLambert*)smat;
						FbxPropertyT<FbxDouble3> p = lam->Emissive;//Diffuse
						FbxDouble3 color = p.Get();
						greenMt = MaterialHelper::CreateMaterial(Color::FromRgb((uchar)(255 * color[0]), (uchar)(255 * color[1]), (uchar)(255 * color[2])));
						break;
					}
				}

				int polygonCount = mesh->GetPolygonCount();
				FbxVector4* controlPoints = mesh->GetControlPoints();
				for (int j = 0; j < polygonCount; j++)
				{
					int vertexCount = mesh->GetPolygonSize(j);
					auto points = gcnew List<Vector3>();
					for (int k = 0; k < vertexCount; k++)
					{
						int vertexIndex = mesh->GetPolygonVertex(j, k);
						FbxVector4 vertex = controlPoints[vertexIndex];
						points->Add(Vector3((float)vertex[0], (float)vertex[1], (float)vertex[2]));
					}
					meshBuilder->AddPolygon(points);
				}
				_modelGroup->Children->Add(
					gcnew GeometryModel3D(ConverterExtensions::ToWndMeshGeometry3D(meshBuilder->ToMesh(), true), greenMt));
			}
		}
		for (int i = 0; i < node->GetChildCount(); i++)
			BuildMeshes(node->GetChild(i));
	}

	MainWnd::~MainWnd()
	{
		delete _fbxLoader;
	}

	void MainWnd::OnOpenFile(Object^ sender, RoutedEventArgs^ e)
	{
		OpenFileDialog^ dlg = gcnew OpenFileDialog();
		if (dlg->ShowDialog())
		{
			delete _fbxLoader;
			_fbxLoader = new FbxLoader();

			string fname = ConvertToStdString(dlg->FileName);
			_fbxLoader->importFile(fname.c_str());

			BuildMeshes((FbxNode*)(_fbxLoader->_scene));

			// some ad hoc transform FIX ME
			Vector3D ra = Vector3D(1, 0, 0);
			AxisAngleRotation3D^ aar = gcnew AxisAngleRotation3D(ra, 90);
			RotateTransform3D^ rt = gcnew RotateTransform3D(aar);
			_modelGroup->Transform = rt;
		}
	}

	void MainWnd::OnUser(Object^ sender, RoutedEventArgs^ e)
	{
	}

}