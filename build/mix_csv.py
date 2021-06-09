import pandas
import numpy

DETECTORS = ["SHITOMASI", "HARRIS", "FAST", "BRISK", "ORB", "AKAZE", "SIFT"]
DESCRIPTORS = ["BRISK", "BRIEF", "ORB", "FREAK", "AKAZE", "SIFT"]

def has_finite_values(df):
    inf = numpy.isinf(numpy.abs(df)).values.sum()
    return not inf

def is_df_valid(df):
    return df.shape[0] == 18 and df.isnull().sum().sum() == 0 and has_finite_values(df)

def process_file(filename):
    try:
        df = pandas.read_csv(filename)
        if is_df_valid(df):
            return df
    except:
        print(f'File {filename} could not be read.')
    return None

def mix_dataframes(dataframes):
    mix_df = pandas.DataFrame()
    mix_df['TTC_LIDAR'] = dataframes[0]['TTC_LIDAR']
    for df in dataframes:
        column_name = df.columns[1]
        mix_df[column_name] = df[column_name]
    return mix_df

def compute_rmse(lidar_ttc, camera_ttc):
    rmse = 0.
    n = 0
    for l, c in zip(lidar_ttc, camera_ttc):
        error = l - c
        rmse += error * error
        n += 1
    rmse = numpy.sqrt(rmse / n)
    return rmse

def compute_all_rmse(mix_df):
    lidar_column = mix_df.columns[0]
    lidar_ttc = mix_df[lidar_column]
    rmse_data = []
    for camera_column in mix_df.columns[1:]:
        camera_ttc = mix_df[camera_column]
        rmse = compute_rmse(lidar_ttc, camera_ttc)
        rmse_data.append((camera_column, rmse))
    rmse_data = sorted(rmse_data, key = lambda e: e[1])
    return rmse_data

def main():
    dataframes = []
    invalid_combinations = []
    for detector in DETECTORS:
        for descriptor in DESCRIPTORS:
            filename = f'{detector}_{descriptor}.csv'
            df = process_file(filename)
            valid = not df is None
            if valid: 
                dataframes.append(df)
            else:
                invalid_combinations.append(filename)
            valid_string = 'VALID' if valid else 'INVALID'
            print(f'{filename} ({valid_string})')
    mix_df = mix_dataframes(dataframes)
    print('mix_df:')
    print(mix_df)
    mix_df.to_csv('RESULTS.CSV', index=False)
    print('Invalid combinations:')
    print(invalid_combinations)
    print('\nRMSE:')
    rmse_data = compute_all_rmse(mix_df)
    for algorithm, rmse in rmse_data:
        print(f'{algorithm} {rmse:.4f}')

if __name__ == "__main__":
    main()