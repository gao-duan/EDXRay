#include "Medium.h"
#include "Graphics/Color.h"

namespace EDX
{
	namespace RayTracer
	{
		float PhaseFunctionHG::Eval(const Vector3& wo, const Vector3& wi) const
		{
			return HenyeyGreenstein(Math::Dot(wo, wi), mG);
		}

		float PhaseFunctionHG::Sample(const Vector3& wo, Vector3* pvIn, const Vector2& sample) const
		{
			float cosTheta;
			if (Math::Abs(mG) < 1e-3)
			{
				cosTheta = 1 - 2 * sample.u;
			}
			else
			{
				float sqrTerm = (1 - mG * mG) / (1 - mG + 2 * mG * sample.u);
				cosTheta = (1 + mG * mG - sqrTerm * sqrTerm) / (2 * mG);
			}

			// Compute direction _wi_ for Henyey--Greenstein sample
			float sinTheta = Math::Sqrt(Math::Max(0.0f, 1 - cosTheta * cosTheta));
			float phi = float(Math::EDX_TWO_PI) * sample.v;

			Vector3 v1, v2;
			Math::CoordinateSystem(wo, &v1, &v2);
			*pvIn = Math::SphericalDirection(sinTheta, cosTheta, phi, v1, -wo, v2);

			return HenyeyGreenstein(-cosTheta, mG);
		}

		
		/*
eta = 1.5;
Fdr = -1.440/(eta.^2) + 0.710/eta + 0.668 + 0.0636*eta;
A = (1 + Fdr)/(1 - Fdr);
alpha = 0:1/99:1;
c = alpha.*(1+exp(-4/3*A*sqrt(3*(1-alpha)))).* exp(-sqrt(3*(1-alpha)));
C = 0:1/999:1;
ALPHA = interp1(c,alpha,C);
for i=1:1000
printf("%df, ", ALPHA(i));
if mod(i,5)==0 printf("\n");
endif
endfor
*/

		float Reparameterizer::ReducedAlbedoLUT[Reparameterizer::LUTSize] = {
			0.0f, 0.00560786f, 0.0111963f, 0.0167065f, 0.0221816f,
			0.0275956f, 0.0329621f, 0.0382812f, 0.043544f, 0.0487695f,
			0.0539334f, 0.0590667f, 0.0641363f, 0.0691786f, 0.0741584f,
			0.079111f, 0.0840055f, 0.0888697f, 0.0936832f, 0.0984602f,
			0.103197f, 0.107888f, 0.112552f, 0.117159f, 0.121755f,
			0.126277f, 0.1308f, 0.135249f, 0.139689f, 0.144079f,
			0.148438f, 0.152773f, 0.157052f, 0.16133f, 0.165535f,
			0.169734f, 0.173892f, 0.178013f, 0.182129f, 0.186173f,
			0.190217f, 0.194218f, 0.198186f, 0.202152f, 0.206045f,
			0.209938f, 0.213799f, 0.217619f, 0.221438f, 0.2252f,
			0.228947f, 0.232686f, 0.236361f, 0.240036f, 0.243686f,
			0.247289f, 0.250893f, 0.254459f, 0.257992f, 0.261526f,
			0.265012f, 0.268477f, 0.271942f, 0.275354f, 0.27875f,
			0.282147f, 0.28549f, 0.288819f, 0.292149f, 0.295427f,
			0.29869f, 0.301953f, 0.305172f, 0.30837f, 0.311568f,
			0.314732f, 0.317865f, 0.320998f, 0.324113f, 0.327182f,
			0.330251f, 0.33332f, 0.336327f, 0.339333f, 0.34234f,
			0.345307f, 0.348251f, 0.351195f, 0.354127f, 0.35701f,
			0.359893f, 0.362776f, 0.365616f, 0.368439f, 0.371261f,
			0.374077f, 0.376839f, 0.379602f, 0.382365f, 0.3851f,
			0.387804f, 0.390508f, 0.393212f, 0.395873f, 0.398519f,
			0.401165f, 0.403811f, 0.406404f, 0.408992f, 0.411581f,
			0.414168f, 0.4167f, 0.419232f, 0.421763f, 0.424294f,
			0.426769f, 0.429245f, 0.431721f, 0.434196f, 0.43662f,
			0.439041f, 0.441461f, 0.443881f, 0.44626f, 0.448626f,
			0.450992f, 0.453357f, 0.455696f, 0.458008f, 0.46032f,
			0.462632f, 0.464937f, 0.467196f, 0.469455f, 0.471714f,
			0.473972f, 0.476196f, 0.478403f, 0.480609f, 0.482815f,
			0.485017f, 0.487171f, 0.489326f, 0.49148f, 0.493634f,
			0.495768f, 0.497871f, 0.499974f, 0.502077f, 0.50418f,
			0.506253f, 0.508306f, 0.510358f, 0.51241f, 0.514462f,
			0.516481f, 0.518484f, 0.520486f, 0.522488f, 0.52449f,
			0.526462f, 0.528414f, 0.530367f, 0.53232f, 0.534273f,
			0.536203f, 0.538107f, 0.540011f, 0.541915f, 0.543819f,
			0.545716f, 0.547571f, 0.549427f, 0.551282f, 0.553138f,
			0.554993f, 0.556815f, 0.558623f, 0.560431f, 0.562238f,
			0.564046f, 0.565849f, 0.567609f, 0.569369f, 0.57113f,
			0.57289f, 0.574651f, 0.576394f, 0.578108f, 0.579821f,
			0.581535f, 0.583249f, 0.584962f, 0.586654f, 0.588322f,
			0.589989f, 0.591657f, 0.593324f, 0.594992f, 0.59664f,
			0.598262f, 0.599884f, 0.601505f, 0.603127f, 0.604749f,
			0.606362f, 0.607939f, 0.609515f, 0.611092f, 0.612668f,
			0.614245f, 0.615821f, 0.617363f, 0.618895f, 0.620426f,
			0.621958f, 0.62349f, 0.625022f, 0.626545f, 0.628032f,
			0.62952f, 0.631007f, 0.632495f, 0.633982f, 0.63547f,
			0.63694f, 0.638383f, 0.639827f, 0.64127f, 0.642714f,
			0.644158f, 0.645601f, 0.647027f, 0.648427f, 0.649827f,
			0.651227f, 0.652628f, 0.654028f, 0.655428f, 0.65682f,
			0.658177f, 0.659534f, 0.660891f, 0.662248f, 0.663605f,
			0.664962f, 0.666319f, 0.667644f, 0.668959f, 0.670273f,
			0.671587f, 0.672902f, 0.674216f, 0.67553f, 0.676842f,
			0.678114f, 0.679386f, 0.680658f, 0.68193f, 0.683202f,
			0.684474f, 0.685746f, 0.687013f, 0.688243f, 0.689473f,
			0.690703f, 0.691933f, 0.693163f, 0.694393f, 0.695623f,
			0.696853f, 0.698045f, 0.699234f, 0.700422f, 0.70161f,
			0.702799f, 0.703987f, 0.705176f, 0.706364f, 0.707535f,
			0.708682f, 0.709829f, 0.710976f, 0.712123f, 0.71327f,
			0.714417f, 0.715564f, 0.716711f, 0.717834f, 0.718939f,
			0.720045f, 0.721151f, 0.722257f, 0.723363f, 0.724469f,
			0.725575f, 0.726681f, 0.727767f, 0.728832f, 0.729898f,
			0.730963f, 0.732028f, 0.733093f, 0.734158f, 0.735223f,
			0.736288f, 0.737353f, 0.738378f, 0.739402f, 0.740427f,
			0.741451f, 0.742476f, 0.7435f, 0.744524f, 0.745549f,
			0.746573f, 0.747593f, 0.748577f, 0.749561f, 0.750545f,
			0.751529f, 0.752513f, 0.753497f, 0.754481f, 0.755465f,
			0.756449f, 0.757433f, 0.758382f, 0.759326f, 0.76027f,
			0.761213f, 0.762157f, 0.763101f, 0.764045f, 0.764988f,
			0.765932f, 0.766876f, 0.767813f, 0.768717f, 0.769621f,
			0.770524f, 0.771428f, 0.772332f, 0.773235f, 0.774139f,
			0.775042f, 0.775946f, 0.77685f, 0.777753f, 0.778618f,
			0.779482f, 0.780345f, 0.781209f, 0.782072f, 0.782936f,
			0.7838f, 0.784663f, 0.785527f, 0.78639f, 0.787254f,
			0.788106f, 0.78893f, 0.789754f, 0.790577f, 0.791401f,
			0.792225f, 0.793048f, 0.793872f, 0.794696f, 0.795519f,
			0.796343f, 0.797167f, 0.79799f, 0.798774f, 0.799557f,
			0.800341f, 0.801125f, 0.801909f, 0.802692f, 0.803476f,
			0.80426f, 0.805044f, 0.805827f, 0.806611f, 0.807395f,
			0.808174f, 0.808918f, 0.809661f, 0.810405f, 0.811149f,
			0.811893f, 0.812637f, 0.813381f, 0.814124f, 0.814868f,
			0.815612f, 0.816356f, 0.8171f, 0.817844f, 0.818566f,
			0.81927f, 0.819973f, 0.820677f, 0.821381f, 0.822085f,
			0.822789f, 0.823493f, 0.824197f, 0.824901f, 0.825604f,
			0.826308f, 0.827012f, 0.827716f, 0.828412f, 0.829076f,
			0.82974f, 0.830404f, 0.831068f, 0.831731f, 0.832395f,
			0.833059f, 0.833723f, 0.834387f, 0.835051f, 0.835714f,
			0.836378f, 0.837042f, 0.837706f, 0.83837f, 0.838994f,
			0.839618f, 0.840242f, 0.840865f, 0.841489f, 0.842113f,
			0.842737f, 0.84336f, 0.843984f, 0.844608f, 0.845231f,
			0.845855f, 0.846479f, 0.847102f, 0.847726f, 0.84835f,
			0.848942f, 0.849525f, 0.850109f, 0.850692f, 0.851275f,
			0.851859f, 0.852442f, 0.853025f, 0.853609f, 0.854192f,
			0.854775f, 0.855359f, 0.855942f, 0.856526f, 0.857109f,
			0.857692f, 0.858276f, 0.85884f, 0.859383f, 0.859926f,
			0.860469f, 0.861011f, 0.861554f, 0.862097f, 0.86264f,
			0.863183f, 0.863726f, 0.864268f, 0.864811f, 0.865354f,
			0.865897f, 0.86644f, 0.866983f, 0.867525f, 0.868068f,
			0.868611f, 0.869119f, 0.869621f, 0.870123f, 0.870625f,
			0.871127f, 0.871629f, 0.872131f, 0.872634f, 0.873136f,
			0.873638f, 0.87414f, 0.874642f, 0.875144f, 0.875646f,
			0.876148f, 0.87665f, 0.877152f, 0.877654f, 0.878157f,
			0.878659f, 0.87913f, 0.879591f, 0.880052f, 0.880514f,
			0.880975f, 0.881436f, 0.881897f, 0.882358f, 0.882819f,
			0.88328f, 0.883741f, 0.884202f, 0.884663f, 0.885124f,
			0.885585f, 0.886047f, 0.886508f, 0.886969f, 0.88743f,
			0.887891f, 0.888352f, 0.888813f, 0.88924f, 0.889659f,
			0.890079f, 0.890499f, 0.890919f, 0.891339f, 0.891759f,
			0.892178f, 0.892598f, 0.893018f, 0.893438f, 0.893858f,
			0.894277f, 0.894697f, 0.895117f, 0.895537f, 0.895957f,
			0.896377f, 0.896796f, 0.897216f, 0.897636f, 0.898056f,
			0.898476f, 0.898895f, 0.899283f, 0.899661f, 0.90004f,
			0.900418f, 0.900796f, 0.901175f, 0.901553f, 0.901931f,
			0.902309f, 0.902688f, 0.903066f, 0.903444f, 0.903823f,
			0.904201f, 0.904579f, 0.904957f, 0.905336f, 0.905714f,
			0.906092f, 0.906471f, 0.906849f, 0.907227f, 0.907606f,
			0.907984f, 0.908362f, 0.90874f, 0.909116f, 0.909452f,
			0.909789f, 0.910125f, 0.910462f, 0.910798f, 0.911135f,
			0.911471f, 0.911808f, 0.912145f, 0.912481f, 0.912818f,
			0.913154f, 0.913491f, 0.913827f, 0.914164f, 0.9145f,
			0.914837f, 0.915174f, 0.91551f, 0.915847f, 0.916183f,
			0.91652f, 0.916856f, 0.917193f, 0.917529f, 0.917866f,
			0.918203f, 0.918539f, 0.918876f, 0.91921f, 0.919504f,
			0.919799f, 0.920094f, 0.920388f, 0.920683f, 0.920978f,
			0.921272f, 0.921567f, 0.921862f, 0.922156f, 0.922451f,
			0.922746f, 0.92304f, 0.923335f, 0.92363f, 0.923924f,
			0.924219f, 0.924514f, 0.924808f, 0.925103f, 0.925398f,
			0.925692f, 0.925987f, 0.926282f, 0.926576f, 0.926871f,
			0.927166f, 0.927461f, 0.927755f, 0.92805f, 0.928345f,
			0.928639f, 0.928934f, 0.929229f, 0.92949f, 0.929743f,
			0.929996f, 0.930249f, 0.930502f, 0.930754f, 0.931007f,
			0.93126f, 0.931513f, 0.931766f, 0.932018f, 0.932271f,
			0.932524f, 0.932777f, 0.933029f, 0.933282f, 0.933535f,
			0.933788f, 0.934041f, 0.934293f, 0.934546f, 0.934799f,
			0.935052f, 0.935305f, 0.935557f, 0.93581f, 0.936063f,
			0.936316f, 0.936568f, 0.936821f, 0.937074f, 0.937327f,
			0.93758f, 0.937832f, 0.938085f, 0.938338f, 0.938591f,
			0.938844f, 0.939096f, 0.939349f, 0.939568f, 0.939779f,
			0.93999f, 0.940201f, 0.940412f, 0.940623f, 0.940834f,
			0.941045f, 0.941256f, 0.941467f, 0.941678f, 0.94189f,
			0.942101f, 0.942312f, 0.942523f, 0.942734f, 0.942945f,
			0.943156f, 0.943367f, 0.943578f, 0.943789f, 0.944f,
			0.944212f, 0.944423f, 0.944634f, 0.944845f, 0.945056f,
			0.945267f, 0.945478f, 0.945689f, 0.9459f, 0.946111f,
			0.946322f, 0.946534f, 0.946745f, 0.946956f, 0.947167f,
			0.947378f, 0.947589f, 0.9478f, 0.948011f, 0.948222f,
			0.948433f, 0.948645f, 0.948856f, 0.949067f, 0.949278f,
			0.949489f, 0.94966f, 0.94983f, 0.95f, 0.95017f,
			0.95034f, 0.950509f, 0.950679f, 0.950849f, 0.951019f,
			0.951189f, 0.951359f, 0.951529f, 0.951699f, 0.951869f,
			0.952038f, 0.952208f, 0.952378f, 0.952548f, 0.952718f,
			0.952888f, 0.953058f, 0.953228f, 0.953398f, 0.953567f,
			0.953737f, 0.953907f, 0.954077f, 0.954247f, 0.954417f,
			0.954587f, 0.954757f, 0.954927f, 0.955096f, 0.955266f,
			0.955436f, 0.955606f, 0.955776f, 0.955946f, 0.956116f,
			0.956286f, 0.956456f, 0.956625f, 0.956795f, 0.956965f,
			0.957135f, 0.957305f, 0.957475f, 0.957645f, 0.957815f,
			0.957985f, 0.958154f, 0.958324f, 0.958494f, 0.958664f,
			0.958834f, 0.959004f, 0.959174f, 0.959344f, 0.959514f,
			0.959663f, 0.959792f, 0.959922f, 0.960051f, 0.960181f,
			0.960311f, 0.96044f, 0.96057f, 0.960699f, 0.960829f,
			0.960959f, 0.961088f, 0.961218f, 0.961347f, 0.961477f,
			0.961606f, 0.961736f, 0.961866f, 0.961995f, 0.962125f,
			0.962254f, 0.962384f, 0.962514f, 0.962643f, 0.962773f,
			0.962902f, 0.963032f, 0.963162f, 0.963291f, 0.963421f,
			0.96355f, 0.96368f, 0.963809f, 0.963939f, 0.964069f,
			0.964198f, 0.964328f, 0.964457f, 0.964587f, 0.964717f,
			0.964846f, 0.964976f, 0.965105f, 0.965235f, 0.965364f,
			0.965494f, 0.965624f, 0.965753f, 0.965883f, 0.966012f,
			0.966142f, 0.966272f, 0.966401f, 0.966531f, 0.96666f,
			0.96679f, 0.966919f, 0.967049f, 0.967179f, 0.967308f,
			0.967438f, 0.967567f, 0.967697f, 0.967827f, 0.967956f,
			0.968086f, 0.968215f, 0.968345f, 0.968474f, 0.968604f,
			0.968734f, 0.968863f, 0.968993f, 0.969122f, 0.969252f,
			0.969382f, 0.969511f, 0.969641f, 0.969748f, 0.969839f,
			0.96993f, 0.97002f, 0.970111f, 0.970202f, 0.970292f,
			0.970383f, 0.970474f, 0.970564f, 0.970655f, 0.970746f,
			0.970836f, 0.970927f, 0.971018f, 0.971108f, 0.971199f,
			0.97129f, 0.97138f, 0.971471f, 0.971562f, 0.971652f,
			0.971743f, 0.971834f, 0.971924f, 0.972015f, 0.972106f,
			0.972197f, 0.972287f, 0.972378f, 0.972469f, 0.972559f,
			0.97265f, 0.972741f, 0.972831f, 0.972922f, 0.973013f,
			0.973103f, 0.973194f, 0.973285f, 0.973375f, 0.973466f,
			0.973557f, 0.973647f, 0.973738f, 0.973829f, 0.973919f,
			0.97401f, 0.974101f, 0.974191f, 0.974282f, 0.974373f,
			0.974463f, 0.974554f, 0.974645f, 0.974735f, 0.974826f,
			0.974917f, 0.975007f, 0.975098f, 0.975189f, 0.975279f,
			0.97537f, 0.975461f, 0.975551f, 0.975642f, 0.975733f,
			0.975823f, 0.975914f, 0.976005f, 0.976095f, 0.976186f,
			0.976277f, 0.976368f, 0.976458f, 0.976549f, 0.97664f,
			0.97673f, 0.976821f, 0.976912f, 0.977002f, 0.977093f,
			0.977184f, 0.977274f, 0.977365f, 0.977456f, 0.977546f,
			0.977637f, 0.977728f, 0.977818f, 0.977909f, 0.978f,
			0.97809f, 0.978181f, 0.978272f, 0.978362f, 0.978453f,
			0.978544f, 0.978634f, 0.978725f, 0.978816f, 0.978906f,
			0.978997f, 0.979088f, 0.979178f, 0.979269f, 0.97936f,
			0.97945f, 0.979541f, 0.979632f, 0.979722f, 0.979807f,
			0.979861f, 0.979914f, 0.979968f, 0.980021f, 0.980075f,
			0.980128f, 0.980182f, 0.980236f, 0.980289f, 0.980343f,
			0.980396f, 0.98045f, 0.980503f, 0.980557f, 0.980611f,
			0.980664f, 0.980718f, 0.980771f, 0.980825f, 0.980878f,
			0.980932f, 0.980986f, 0.981039f, 0.981093f, 0.981146f
		};

		void Reparameterizer::Eval(const Color& diffuseReflectance, const Vector3& diffuseMeanFreePath, const float eta, Vector3* pSigmaS, Vector3* pSigmaA)
		{
			// Secant root finder, suffered from numerical issues, disabled for now.
			//float F_dr = Fresnel_dr(eta);
			//float A = InternalReflectParam(F_dr);

			//Vector3 alpha[3] = { Vector3(0.5f), Vector3(0.6f), Vector3::ZERO };
			//for (auto i = 0; i < 7; i++)
			//{
			//	Vector3 F0 = DiffuseReflectance(diffuseReflectance, alpha[i % 3], A);
			//	Vector3 F1 = DiffuseReflectance(diffuseReflectance, alpha[(i + 1) % 3], A);
			//	alpha[(i + 2) % 3] = alpha[(i + 1) % 3] - F1 * (alpha[(i + 1) % 3] - alpha[i % 3]) / (F1 - F0);

			//	alpha[(i + 2) % 3].x = Math::Saturate(alpha[(i + 2) % 3].x);
			//	alpha[(i + 2) % 3].y = Math::Saturate(alpha[(i + 2) % 3].y);
			//	alpha[(i + 2) % 3].z = Math::Saturate(alpha[(i + 2) % 3].z);
			//}

			// Instead simply look up from a precomputed table. The IOR is hard coded to be 1.5
			Vector3 alpha_p = Vector3(
				ReducedAlbedoLUT[Math::RoundToInt(Math::Min(diffuseReflectance.r * Reparameterizer::LUTSize, Reparameterizer::LUTSize - 1))],
				ReducedAlbedoLUT[Math::RoundToInt(Math::Min(diffuseReflectance.g * Reparameterizer::LUTSize, Reparameterizer::LUTSize - 1))],
				ReducedAlbedoLUT[Math::RoundToInt(Math::Min(diffuseReflectance.b * Reparameterizer::LUTSize, Reparameterizer::LUTSize - 1))]
				);
			Vector3 sigma_tr = Vector3(1.0f) / diffuseMeanFreePath;
			Vector3 sigma_t_p = sigma_tr / Math::Sqrt(3.0f * (Vector3(1.0f) - alpha_p));

			*pSigmaS = alpha_p * sigma_t_p;
			*pSigmaA = sigma_t_p - *pSigmaS;
		}

		Vector3 Reparameterizer::DiffuseReflectance(const Color& diffuseReflectance, const Vector3& alpha, const float A)
		{
			Vector3 term = Math::Sqrt(3.0f * (Vector3(1.0f) - alpha));
			return 0.5f * alpha * (Vector3(1.0f) + Math::Exp((-4.0f / 3.0f) * A * term)) * Math::Exp(-term) - Vector3(diffuseReflectance.r, diffuseReflectance.g, diffuseReflectance.b);
		}

		float Reparameterizer::InternalReflectParam(const float F_dr)
		{
			return (1.0f + F_dr) / (1.0f - F_dr);
		}

		float Reparameterizer::Fresnel_dr(const float eta)
		{
			return -(1.44f / (eta * eta)) + (0.71f / eta) + 0.668f + 0.0636 * eta;
		}
	}
}